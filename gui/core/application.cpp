//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <thread>

#include "ot/function.h"
#include "ot/libuv.h"
#include "ot/vm.h"

#include "application.h"


//
//	OtApplicationClass::run
//

void OtApplicationClass::run(const std::string& name) {
	// applications run in two threads: the main thread handles the rendering
	// and window events (as required in most operating systems)
	// the second thread runs the application logic

	// initialize window library
	initGLFW(name);

	// start the second thread
	running = true;

	std::thread thread([this]() {
		// run the second thread
		this->runThread2();
		return 0;
	});

	// application main loop
	while (runningGLFW()) {
		// handle window events
		eventsGLFW();
	}

	// wait for thread to finish
	running = false;
	thread.join();

	// close windowing library
	endGLFW();
}


//
//	OtApplicationClass::runThread2
//

void OtApplicationClass::runThread2() {
	try {
		// initialize libraries
		initBGFX();
		initIMGUI();

		// create a screen widget
		screen = OtScreenClass::create();

		// call init callbacks
		for (auto& callback : atInitCallbacks) {
			callback();
		}

		// call app's setup member (if defined)
		if (has("setup")) {
			OtVM::instance()->callMemberFunction(shared(), "setup", screen);
		}

		// application main loop
		while (running) {
			// update frame number
			frameNumber++;

			// reset next view ID
			nextViewID = 1;

			// update all animations
			for (int c = animations.size() - 1; c >= 0; c--) {
				if (!animations[c]->step(loopDuration)) {
					animations.erase(animations.begin() + c);
				}
			}

			// update all simulations
			for (auto& simulation : simulations) {
				if (simulation->isRunning()) {
					simulation->step(loopDuration);
				}
			}

			// call app's update member (if defined)
			if (has("update")) {
				OtVM::instance()->callMemberFunction(shared(), "update");
			}

			// collect events
			std::vector<OtAppEvent> events;

			while (!eventQueue.empty()) {
				events.emplace_back(eventQueue.pop());
			}

			// start new frame in libraries
			frameBGFX();
			frameIMGUI(events);

			// submit events (if still required; ImGui might take some)
			for (auto& event : events) {
				switch (event.type) {
					case OtAppEvent::mouseButtonEvent:
						if (has("onMouseButton")) {
							OtVM::instance()->callMemberFunction(shared(), "onMouseButton",
								OtObjectCreate(event.mouseButton.button),
								OtObjectCreate(event.mouseButton.action),
								OtObjectCreate(event.mouseButton.mods),
								OtObjectCreate(event.mouseButton.x),
								OtObjectCreate(event.mouseButton.y));

						} else {
							screen->onMouseButton(
								event.mouseButton.button,
								event.mouseButton.action,
								event.mouseButton.mods,
								event.mouseButton.x,
								event.mouseButton.y);
						}

						break;

					case OtAppEvent::mouseMoveEvent:
						if (has("onMouseMove")) {
							OtVM::instance()->callMemberFunction(shared(), "onMouseMove",
								OtObjectCreate(event.mouseMove.x),
								OtObjectCreate(event.mouseMove.y));

						} else {
							screen->onMouseMove(event.mouseMove.x, event.mouseMove.y);
						}

						break;

					case OtAppEvent::mouseDragEvent:
						if (has("onMouseDrag")) {
							OtVM::instance()->callMemberFunction(shared(), "onMouseDrag",
								OtObjectCreate(event.mouseDrag.button),
								OtObjectCreate(event.mouseDrag.mods),
								OtObjectCreate(event.mouseDrag.x),
								OtObjectCreate(event.mouseDrag.y));

						} else {
							screen->onMouseDrag(
									event.mouseDrag.button,
									event.mouseDrag.mods,
									event.mouseDrag.x,
									event.mouseDrag.y);
						}

						break;

					case OtAppEvent::mouseWheelEvent:
						if (has("onScrollWheel")) {
							OtVM::instance()->callMemberFunction(shared(), "onScrollWheel",
								OtObjectCreate(event.mouseWheel.xOffset),
								OtObjectCreate(event.mouseWheel.yOffset));

						} else {
							screen->onScrollWheel(
								event.mouseWheel.xOffset,
								event.mouseWheel.yOffset);
						}

						break;

					case OtAppEvent::keyboardEvent:
						if (event.keyboard.action != GLFW_RELEASE) {
							bool handled = false;

							for (auto& shortcut : shortcuts) {
								if (shortcut.modifier == event.keyboard.mods && shortcut.key == event.keyboard.key) {
									shortcut.callback();
									handled = true;
								}
							}

							if (!handled) {
								if (has("onKey")) {
									OtVM::instance()->callMemberFunction(shared(), "onKey", OtObjectCreate(event.keyboard.key), OtObjectCreate(event.keyboard.mods));

								} else {
									screen->onKey(event.keyboard.key, event.keyboard.mods);
								}
							}
						}
						break;

					case OtAppEvent::characterEvent:
						if (has("onChar")) {
							OtVM::instance()->callMemberFunction(shared(), "onChar", OtObjectCreate(event.character.codepoint));

						} else {
							screen->onChar(event.character.codepoint);
						}

						break;

					case OtAppEvent::gamepadAxisEvent:
						if (has("onGamepadAxis")) {
							OtVM::instance()->callMemberFunction(shared(), "onGamepadAxis",
								OtObjectCreate(event.gamepadAxis.gamepad),
								OtObjectCreate(event.gamepadAxis.axis),
								OtObjectCreate(event.gamepadAxis.value));

						} else {
							screen->onGamepadAxis(
								event.gamepadAxis.gamepad,
								event.gamepadAxis.axis,
								event.gamepadAxis.value);
						}

						break;

					case OtAppEvent::gamepadButtonEvent:
						if (has("onGamepadButton")) {
							OtVM::instance()->callMemberFunction(shared(), "onGamepadButton",
								OtObjectCreate(event.gamepadButton.gamepad),
								OtObjectCreate(event.gamepadButton.button),
								OtObjectCreate(event.gamepadButton.action));

						} else {
							screen->onGamepadButton(
								event.gamepadButton.gamepad,
								event.gamepadButton.button,
								event.gamepadButton.action);
						}

						break;
				}
			}

			// update and render all elements
			screen->update();
			screen->render();

			// call render callbacks
			for (auto& callback : atRenderCallbacks) {
				callback();
			}

			// add profiler (if required)
			if (profiler) {
				renderProfiler();
			}

			// put results on screen
			renderIMGUI();
			renderBGFX();

			// handle libuv events
			uv_run(uv_default_loop(), UV_RUN_NOWAIT);
		}

		// call app's update member (if defined)
		if (has("terminate")) {
			OtVM::instance()->callMemberFunction(shared(), "terminate");
		}

		// remove all animations and simulations
		animations.clear();
		simulations.clear();

		// remove all children from the screen to avoid memory leaks
		// there are circular parent/child relationships
		screen->clear();
		screen = nullptr;

		// also remove app instance variables that the user might have added
		unsetAll();

		// call exit callbacks
		for (auto& callback : atExitCallbacks) {
			callback();
		}

		// terminate libraries
		endIMGUI();
		endBGFX();

		// properly close all libuv handles
		uv_walk(uv_default_loop(), [](uv_handle_t* handle, void* arg) {
			if (!uv_is_closing(handle))
				uv_close(handle, nullptr);
		}, nullptr);

		uv_run(uv_default_loop(), UV_RUN_DEFAULT);
		uv_loop_close(uv_default_loop());

	} catch (const OtException& e) {
		// handle all failures
		std::wcerr << "Error: " << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}
}


//
//	OtApplicationClass::quit
//

void OtApplicationClass::quit() {
	// quiting is realized by closing the app's window
	quitGLFW();
}

//
//	OtApplicationClass::setAntiAliasing
//

OtObject OtApplicationClass::setAntiAliasing(int aa) {
	if (aa < 0 || aa > 4) {
		OtExcept("Anti-aliasing setting must be between 0 and 4");
	}

	antiAliasing = aa;
	return shared();
}


//
//	OtApplicationClass::setProfiler
//

OtObject OtApplicationClass::setProfiler(bool profilerState) {
	profiler = profilerState;
	return shared();
}


//
//	OtApplicationClass::animate
//

OtObject OtApplicationClass::animation() {
	OtAnimation animation = OtAnimationClass::create();
	animations.push_back(animation);
	return animation;
}


//
//	OtApplicationClass::addSimulation
//

OtObject OtApplicationClass::addSimulation(OtObject object) {
	// ensure object is a simulation
	if (object->isKindOf("Simulation")) {
		simulations.push_back(object->cast<OtSimulationClass>());

	} else {
		OtExcept("Expected a [Simulation] object, not a [%s]", object->getType()->getName().c_str());
	}

	return object;
}


//
//	OtApplicationClass::addShortcut
//

void OtApplicationClass::addShortcut(int modifier, int keycode, std::function<void(void)> callback) {
	shortcuts.push_back(OtKeyboardShortcut(modifier, keycode, callback));
}


//
//	OtApplicationClass::getMeta
//

OtType OtApplicationClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtApplicationClass>(
			"Application",
			OtGuiClass::getMeta(),
			[]() { return (OtObject) OtApplicationClass::instance(); });

		type->set("run", OtFunctionClass::create(&OtApplicationClass::run));
		type->set("animation", OtFunctionClass::create(&OtApplicationClass::animation));
		type->set("addSimulation", OtFunctionClass::create(&OtApplicationClass::addSimulation));
		type->set("setAntiAliasing", OtFunctionClass::create(&OtApplicationClass::setAntiAliasing));
		type->set("setProfiler", OtFunctionClass::create(&OtApplicationClass::setProfiler));

		type->set("getWidth", OtFunctionClass::create(&OtApplicationClass::getWidth));
		type->set("getHeight", OtFunctionClass::create(&OtApplicationClass::getHeight));
	}

	return type;
}


//
//	OtApplicationClass::create
//

OtApplication OtApplicationClass::create() {
	return OtApplicationClass::instance();
}
