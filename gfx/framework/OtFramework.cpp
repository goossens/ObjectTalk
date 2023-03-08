//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cstdlib>
#include <thread>

#include "OtCerr.h"
#include "OtException.h"
#include "OtFunction.h"
#include "OtLibuv.h"
#include "OtVM.h"

#include "OtFramework.h"
#include "OtMessageBus.h"
#include "OtPass.h"


//
//	OtFrameworkClass::run
//

void OtFrameworkClass::run(OtFrameworkApp* targetApp) {
	// remember the app
	app = targetApp;

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
//	OtFrameworkClass::runThread2
//

void OtFrameworkClass::runThread2() {
	try {
		// initialize graphics libraries
		initBGFX();
		initIMGUI();

		// call setup callback
		app->onSetup();

		// run this thread until we are told to stop
		while (running) {
			// process all messages on the bus
			OtMessageBus::instance()->process();

			// reset view ID
			OtPassReset();

			// collect events
			std::vector<OtFwEvent> events;

			while (!eventQueue.empty()) {
				events.emplace_back(eventQueue.pop());
			}

			// start new frame
			frameBGFX();
			frameIMGUI(events);

			// submit events (if still required; ImGui might take some)
			for (auto& event : events) {
				bool handled = false;

				switch (event.type) {
					case OtFwEvent::mouseButtonEvent:
						handled = app->onMouseButton(
							event.mouseButton.button,
							event.mouseButton.action,
							event.mouseButton.mods,
							event.mouseButton.x,
							event.mouseButton.y);

						break;

					case OtFwEvent::mouseMoveEvent:
						handled = app->onMouseMove(event.mouseMove.x, event.mouseMove.y);
						break;

					case OtFwEvent::mouseDragEvent:
						handled = app->onMouseDrag(
							event.mouseDrag.button,
							event.mouseDrag.mods,
							event.mouseDrag.x,
							event.mouseDrag.y);

						break;

					case OtFwEvent::mouseWheelEvent:
						handled = app->onScrollWheel(event.mouseWheel.xOffset, event.mouseWheel.yOffset);
						break;

					case OtFwEvent::keyboardEvent:
						if (event.keyboard.action != GLFW_RELEASE) {
							handled = app->onKey(event.keyboard.key, event.keyboard.mods);
						}

						break;

					case OtFwEvent::characterEvent:
						handled = app->onChar(event.character.codepoint);
						break;

					case OtFwEvent::gamepadAxisEvent:
						handled = app->onGamepadAxis(
							event.gamepadAxis.gamepad,
							event.gamepadAxis.axis,
							event.gamepadAxis.value);

						break;

					case OtFwEvent::gamepadButtonEvent:
						handled = app->onGamepadButton(
							event.gamepadButton.gamepad,
							event.gamepadButton.button,
							event.gamepadButton.action);

						break;
				}
			}

			// call update and render callbacks
			app->onUpdate();
			app->onRender();

			// show profiler (if required)
			if (profiler) {
				renderProfiler();
			}

			// put results on screen
			renderIMGUI();
			renderBGFX();

			// handle libuv events
			uv_run(uv_default_loop(), UV_RUN_NOWAIT);
		}

		// clear the message bus
		OtMessageBus::instance()->clear();

		// call termination callback
		app->onTerminate();

		// call exit callbacks
		for (auto& callback : atExitCallbacks) {
			callback();
		}

		// terminate libraries
		endIMGUI();
		endBGFX();

	} catch (const OtException& e) {
		// handle all failures
		std::wcerr << "Error: " << e.what() << std::endl;
			std::_Exit(EXIT_FAILURE);
	}
}


//
//	OtFrameworkClass::stop
//

void OtFrameworkClass::stop() {
	// stopping the framework is realized by closing the app's window
	stopGLFW();
}


//
//	OtFrameworkClass::canQuit
//

bool OtFrameworkClass::canQuit() {
	return app->onCanQuit();
}


//
//	OtFrameworkClass::setAntiAliasing
//

void OtFrameworkClass::setAntiAliasing(int aa) {
	if (aa < 0 || aa > 4) {
		OtExcept("Anti-aliasing setting must be between 0 and 4");
	}

	antiAliasing = aa;
}
