//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
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
#include "theme.h"


//
//	Globals
//

int OtApplicationClass::nextViewID = 1;
size_t OtApplicationClass::frameNumber = 0;

std::vector<OtApplicationClass::OtKeyboardShortcut> OtApplicationClass::shortcuts;

std::vector<std::function<void(void)>> OtApplicationClass::atExitCallbacks;


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
		// initialize theme engine
		OtTheme::init();

		// initialize libraries
		initBGFX();
		initIMGUI();

		// create a screen widget
		screen = OtScreenClass::create();

		// call app's setup member (if defined)
		if (has("setup")) {
			OtVM::callMemberFunction(shared(), "setup", screen);
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
				OtVM::callMemberFunction(shared(), "update");
			}

			// handle events
			clickEvent = false;
			moveEvent = false;
			wheelEvent = false;
			keyEvent = false;
			charEvent = false;

			while (hasEvents()) {
				handleEvent();
			}

			// "run" all libraries
			frameBGFX();
			frameIMGUI();

			// submit events (if required)
			if (clickEvent) {
				screen->onMouseButton(mouseButton, mouseAction, mouseMods, mouseX, mouseY);
			}

			if (moveEvent) {
				if (mouseAction) {
					screen->onMouseDrag(mouseButton, mouseMods, mouseX, mouseY);

				} else {
					screen->onMouseMove(mouseX, mouseY);
				}
			}

			if (wheelEvent) {
				screen->onScrollWheel(mouseWheelDX, mouseWheelDY);
				mouseWheelDX = 0.0;
				mouseWheelDY = 0.0;
			}

			if (keyEvent && keyboardAction != GLFW_RELEASE) {
				bool handled = false;

				for (auto& shortcut : shortcuts) {
					if (shortcut.modifier == keyboardMods && shortcut.keycode == keyboardKey) {
						shortcut.callback();
						handled = true;
					}
				}

				if (!handled) {
					if (has("onKey")) {
						OtVM::callMemberFunction(shared(), "onKey", OtObjectCreate(keyboardKey), OtObjectCreate(keyboardMods));

					} else {
						screen->onKey(keyboardKey, keyboardMods);
					}
				}
			}

			if (charEvent) {
				screen->onChar(keyboardCodepoint);
			}

			// update and render all elements
			screen->update();
			screen->render();

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
			OtVM::callMemberFunction(shared(), "terminate");
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

		// call all atexit callbacks
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
//	OtApplicationClass::pushEvent
//

void OtApplicationClass::pushEvent(const std::string& event) {
	std::lock_guard<std::mutex> guard(lock);
	events.push_back(event);
}


//
//	OtApplicationClass::hasEvents
//

bool OtApplicationClass::hasEvents() {
	std::lock_guard<std::mutex> guard(lock);
	return events.size() != 0;
}


//
//	OtApplicationClass::popEvent
//

std::string OtApplicationClass::popEvent() {
	std::lock_guard<std::mutex> guard(lock);
	std::string event = events.back();
	events.pop_back();
	return event;
}


//
//	OtApplicationClass::handleEvent
//

void OtApplicationClass::handleEvent() {
	auto event = popEvent();

	if (event.find("click") == 0) {
		char command[10];
		std::sscanf(event.c_str(), "%s %d %d %d", command, &mouseButton, &mouseAction, &mouseMods);

		if (mouseButton >= 0 && mouseButton < ImGuiMouseButton_COUNT) {
			mouseButtonState[mouseButton] = mouseAction == GLFW_PRESS;
			clickEvent = true;
		}

	} else if (event.find("move") == 0) {
		char command[10];
		std::sscanf(event.c_str(), "%s %f %f", command, &mouseX, &mouseY);
		moveEvent = true;

	} else if (event.find("wheel") == 0) {
		char command[10];
		float offsetX;
		float offsetY;
		std::sscanf(event.c_str(), "%s %f %f", command, &offsetX, &offsetY);
		mouseWheelDX += offsetX;
		mouseWheelDY += offsetY;
		wheelEvent = true;

	} else if (event.find("key") == 0) {
		char command[10];
		std::sscanf(event.c_str(), "%s %d %d %d %d", command, &keyboardKey, &keyboardScancode, &keyboardAction, &keyboardMods);
		keyboardState[keyboardKey] = keyboardAction != GLFW_RELEASE;
		keyEvent = true;

	} else if (event.find("char") == 0) {
		char command[10];
		std::sscanf(event.c_str(), "%s %d", command, &keyboardCodepoint);
		charEvent = true;
	}
}


//
//	OtApplicationClass::atexit
//

void OtApplicationClass::atexit(std::function<void(void)> callback) {
	atExitCallbacks.push_back(callback);
}


//
//	OtApplicationClass::getMeta
//

OtType OtApplicationClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtApplicationClass>("Application", OtGuiClass::getMeta());
		type->set("run", OtFunctionClass::create(&OtApplicationClass::run));
		type->set("animation", OtFunctionClass::create(&OtApplicationClass::animation));
		type->set("addSimulation", OtFunctionClass::create(&OtApplicationClass::addSimulation));
		type->set("setAntiAliasing", OtFunctionClass::create(&OtApplicationClass::setAntiAliasing));
		type->set("setProfiler", OtFunctionClass::create(&OtApplicationClass::setProfiler));

		type->set("getWidth", OtFunctionClass::create(&OtApplicationClass::getWidth2));
		type->set("getHeight", OtFunctionClass::create(&OtApplicationClass::getHeight2));
	}

	return type;
}


//
//	OtApplicationClass::create
//

OtApplication OtApplicationClass::create() {
	OtApplication application = std::make_shared<OtApplicationClass>();
	application->setType(getMeta());
	return application;
}
