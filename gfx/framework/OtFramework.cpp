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

#include "bgfx/platform.h"

#include "OtConfig.h"
#include "OtException.h"
#include "OtFunction.h"
#include "OtLibuv.h"
#include "OtLog.h"
#include "OtMeasure.h"
#include "OtStderrMultiplexer.h"

#include "OtAnimator.h"
#include "OtAssetManager.h"
#include "OtFramework.h"
#include "OtFrameworkAtExit.h"
#include "OtMessageBus.h"
#include "OtPass.h"


//
//	Globals
//

static std::vector<std::function<void()>> oneTimers;
static std::mutex oneTimersLock;


//
//	OtFramework::run
//

void OtFramework::run(OtFrameworkApp* targetApp) {
	// save the app reference
	app = targetApp;

	// initialize graphics libraries
	initSDL();
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

	// run app until we are told to stop
	running = true;

	while (running) {
		// process all OS events
		eventsSDL();

		// start new frame
		startFrameBGFX();
		startFrameIMGUI();

		// time measurement
		OtMeasureStopWatch stopwatch;

		// process all messages on the bus
		OtMessageBus::process();

		// run all animations
		OtAnimator::update();

		// reset view ID
		OtPassReset();

		// handle libuv events
		// this is done at this point so asyncronous callbacks
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
		endFrameIMGUI();
		endFrameBGFX();
	}

	// tell app we're done
	app->onTerminate();

	// clear the message bus
	OtMessageBus::clear();

	// call exit callbacks
	OtFrameworkAtExit::run();

	// stop the asset manager
	OtAssetManager::stop();

	// terminate libraries
	endIMGUI();
	endBGFX();
	endSDL();
}


//
//	OtFramework::stop
//

void OtFramework::stop() {
	running = false;
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
