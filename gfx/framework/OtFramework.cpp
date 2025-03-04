//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <functional>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#if _WIN32
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#undef APIENTRY
#include <windows.h>
#include <shellapi.h>

#else
#include <stdlib.h>
#endif

#include "OtConfig.h"
#include "OtException.h"
#include "OtFunction.h"
#include "OtLibuv.h"
#include "OtLog.h"
#include "OtMeasure.h"
#include "OtModule.h"
#include "OtStderrMultiplexer.h"
#include "OtVM.h"

#include "OtAnimator.h"
#include "OtAssetManager.h"
#include "OtFramework.h"
#include "OtFrameworkAtExit.h"
#include "OtMessageBus.h"
#include "OtPass.h"


//
//	Globals
//

std::vector<std::string> OtFramework::filesToOpen;
static std::vector<std::function<void()>> oneTimers;
static std::mutex oneTimersLock;


//
//	OtFramework::initialize
//

void OtFramework::initialize() {
	// initialize window library
	initGLFW();
}


//
//	OtFramework::terminate
//

void OtFramework::terminate() {
	// close window library
	endGLFW();
}


//
//	OtFramework::run
//

void OtFramework::run(OtFrameworkApp* targetApp) {
	// remember the app
	app = targetApp;

	// the framework runs in two threads:
	// 1. the main thread handles the rendering and window events (as required by most operating systems)
	// 2. the second thread runs the application logic as well as the asynchronous libuv events
	running = true;

	// start the second thread
	std::thread thread([this]() {
		this->runThread2();
		return 0;
	});

	// main loop to handle window events
	while (runningGLFW()) {
		eventsGLFW();
	}

	// wait for second thread to finish
	running = false;
	thread.join();
}


//
//	OtFramework::runThread2
//

void OtFramework::runThread2() {
	try {
		// initialize graphics libraries
		initBGFX();
		initIMGUI();

		// start the asset manager
		OtAssetManager::start();

		// listen for stop events on the message bus
		OtMessageBus::listen([this](const std::string& message) {
			if (message == "stop") {
				stop();
			}
		});

		// let app perform its own setup
		app->onSetup();

		// run this thread until we are told to stop
		while (running) {
			// start new frame
			frameBGFX();
			frameIMGUI();

			// time measurement
			OtMeasureStopWatch stopwatch;

			// process all messages on the bus
			OtMessageBus::process();

			// run all animations
			OtAnimator::update();

			// reset view ID
			OtPassReset();

			// handle libuv events
			// this is done at this point so that asyncronous callbacks
			// can take part in the rendering process and use the GPU
			uv_run(uv_default_loop(), UV_RUN_NOWAIT);

			// let app render a frame
			app->onRender();

			// calculate CPU time
			cpuTime = stopwatch.elapsed();

			// show profiler (if required)
			if (profiler) {
				renderProfiler();
			}

			// put results on screen
			renderIMGUI();
			renderBGFX();
		}

		// tell app we're done
		app->onTerminate();


		// clear all language releated singletons that might cache objects
		// this is to ensure any UI resources stored in those objects are released now
		OtVM::clear();
		OtModuleClass::clear();

		// clear the message bus
		OtMessageBus::clear();

		// call exit callbacks
		OtFrameworkAtExit::run();

		// stop the asset manager
		OtAssetManager::stop();

		// terminate libraries
		endIMGUI();
		endBGFX();

	} catch (OtException& e) {
		// send exception back to IDE (if required)
		if (OtConfig::inSubprocessMode()) {
			OtStderrMultiplexer::multiplex(e);
		}

		// output human readable text and exit program
		OtLogFatal("Error: {}", e.what());
	}
}


//
//	OtFramework::stop
//

void OtFramework::stop() {
	// stopping the framework is realized by closing the app's window
	stopGLFW();
}


//
//	OtFramework::canQuit
//

bool OtFramework::canQuit() {
	// ask app if we can quit
	return app->onCanQuit();
}


//
//	OtFramework::setAntiAliasing
//

void OtFramework::setAntiAliasing(int aa) {
	if (aa < 0 || aa > 4) {
		OtLogError("Anti-aliasing setting must be between 0 and 4");
	}

	antiAliasing = aa;
}


//
//	OtFramework::openURL
//

#if __APPLE__
	// MacOS version is in OtMacos.mm as it requires Objective-C

#else
void OtFramework::openURL(const std::string& url) {
#if _WIN32
	int size = static_cast<int>(url.size()) + 1;
	WCHAR *temp = new WCHAR[size];
	MultiByteToWideChar(CP_UTF8, 0, url.c_str(), size, temp, size);
	ShellExecuteW(NULL, L"open", temp, NULL, NULL, SW_SHOWNORMAL);
	delete[] temp;

#else
	std::string command = std::string("xdg-open '").append(url).append("'");
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
	system(command.c_str());
#pragma GCC diagnostic pop
#endif
}

#endif
