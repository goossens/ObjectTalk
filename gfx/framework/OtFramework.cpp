//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cstdlib>
#include <iostream>
#include <thread>

#include "OtCerr.h"
#include "OtException.h"
#include "OtFunction.h"
#include "OtLibuv.h"
#include "OtLog.h"
#include "OtMeasure.h"
#include "OtVM.h"

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

void OtFramework::run(OtFrameworkApp* targetApp, bool child) {
	// remember the app and the child mode
	app = targetApp;
	childMode = child;

	// the framework runs in two threads:
	// 1. the main thread handles the rendering and window events (as required by most operating systems)
	// 2. the second thread runs the application logic as well as the asynchronous libuv events
	running = true;

	// initialize window library
	initGLFW();

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

	// close window library
	endGLFW();
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
		OtAssetManager::instance()->start();
		
		// listen for stop events on the message bus
		auto bus = OtMessageBus::instance();

		bus->listen([this](const std::string& message) {
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
			bus->process();

			// reset view ID
			OtPassReset();

			// handle libuv events
			// this is done at this point so that asyncronous callbacks
			// can take part in the rendering process and use the GPU
			uv_run(uv_default_loop(), UV_RUN_NOWAIT);

			// let app render a frame
			app->onRender();

			// calculate CPU time
			cpuTime = stopwatch.getTime();

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

		// clear the message bus
		bus->clear();

		// call exit callbacks
		OtFrameworkAtExit::instance()->run();

		// stop the asset manager
		OtAssetManager::instance()->stop();

		// terminate libraries
		endIMGUI();
		endBGFX();

	} catch (OtException& e) {
		// handle all failures
		if (childMode) {
			// serialize exception and send it to the IDE that started us
			// (wrapped in STX (start of text) and ETX (end of text) ASCII codes)
			std::cerr << '\x02' << e.serialize() << '\x03';
		}

		std::cerr << std::endl << "Error: " << e.what() << std::endl;
		std::_Exit(EXIT_FAILURE);
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
		OtLogFatal("Anti-aliasing setting must be between 0 and 4");
	}

	antiAliasing = aa;
}
