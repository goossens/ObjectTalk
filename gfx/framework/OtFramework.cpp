//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
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
#include "OtStderrMultiplexer.h"

#include "OtAnimationModule.h"
#include "OtAssetManager.h"
#include "OtFramework.h"
#include "OtFrameworkAtExit.h"
#include "OtGpu.h"
#include "OtMessageBus.h"


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
	initIMGUI();

	// start the asset manager
	OtAssetManager::start();

	// listen for stop events on the message bus
	OtMessageBus::listen([this](const std::string& message) {
		if (message == "stop") {
			stop();
		}
	});

	// startup/setup app
	auto& gpu = OtGpu::instance();

	gpu.startup([&]() {
		app->onSetup();
	});

	// start loop timer
	lastTime = std::chrono::high_resolution_clock::now();

	// run app until we are told to stop
	running = true;

	while (running) {
		// process all events
		eventsSDL();

		// calculate loop speed
		loopTime = std::chrono::high_resolution_clock::now();
		auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(loopTime - lastTime).count();
		loopDuration = static_cast<float>(microseconds) / 1000.0f;
		lastTime = loopTime;

		// start new frame
		{
			OtMeasureStopWatch stopwatch;
			gpu.startFrame();
			gpuWaitTime = stopwatch.elapsed();
		}

		startFrameIMGUI();

		// process all messages on the bus
		OtMessageBus::process();

		// run all animations
		OtAnimationClass::update();

		// handle libuv events
		// this is done at this point so asynchronous callbacks
		// can take part in the rendering process and use the GPU
		uv_run(uv_default_loop(), UV_RUN_NOWAIT);

		// let app render a frame
		{
			OtMeasureStopWatch stopwatch;
			app->onRender();
			cpuTime = stopwatch.elapsed();
		}

		// put results on screen
		{
			OtMeasureStopWatch stopwatch;
			endFrameIMGUI();
			gpu.endFrame();
			gpuTime = stopwatch.elapsed();
		}
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
	endSDL();
}


//
//	OtFramework::stop
//

void OtFramework::stop() {
	running = false;
}


//
//	OtFramework::canClose
//

bool OtFramework::canClose() {
	// ask app if we can close outer window and quit
	return app->onCanClose();
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
