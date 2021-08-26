//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/function.h"
#include "ot/libuv.h"
#include "ot/vm.h"

#include "application.h"
#include "theme.h"


//
//	OtApplicationClass::run
//

void OtApplicationClass::run(const std::string& name) {
	// initialize theme engine
	OtTheme::init();

	// initialize libraries
	initGLFW(name);
	initBGFX();
	initIMGUI();

	// create a screen widget
	screen = OtScreenClass::create();

	// call app's setup member (if defined)
	if (has("setup")) {
		OtVM::callMemberFunction(shared(), "setup", screen);
	}

	// application main loop
	while (!glfwWindowShouldClose(window)) {
		// update loop timings
		timeGLFW();

		// update all animations
		for (int c = animations.size() - 1; c >= 0; c--) {
			if (!animations[c]->step(loopDuration * 1000)) {
				animations.erase(animations.begin() + c);
			}
		}

		// call app's update member (if defined)
		if (has("update")) {
			OtVM::callMemberFunction(shared(), "update");
		}

		// render frame
		render();

		// handle window events
		eventsGLFW();

		// handle libuv events
		uv_run(uv_default_loop(), UV_RUN_NOWAIT);
	}

	// call app's update member (if defined)
	if (has("terminate")) {
		OtVM::callMemberFunction(shared(), "terminate");
	}

	// remove all children from the screen to avoid memory leaks
	// it has circular parent/child relationships

	screen->clear();
	screen = nullptr;
	unsetAll();

	// remove all animations
	animations.clear();

	// terminate libraries
	endIMGUI();
	endBGFX();
	endGLFW();

	// properly close all libuv handles
	uv_walk(uv_default_loop(), [](uv_handle_t* handle, void* arg) {
		if (!uv_is_closing(handle))
			uv_close(handle, nullptr);
	}, nullptr);

	uv_run(uv_default_loop(), UV_RUN_DEFAULT);
	uv_loop_close(uv_default_loop());
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
//	OtApplicationClass::render
//

void OtApplicationClass::render() {
	// "run" all libraries
	frameGLFW();
	frameBGFX();
	frameIMGUI();

	// update and render all elements
	screen->update();
	screen->render();

	// put results on screen
	renderIMGUI();
	renderBGFX();
}


//
//	OtApplicationClass::onMouseButton
//

void OtApplicationClass::onMouseButton(int button, int action, int mods, double xpos, double ypos) {
	// see if IMGUI wants the mouse event
	if (!mouseIMGUI()) {
		screen->onMouseButton(button, action, mods, xpos, ypos);
	}
}


//
//	OtApplicationClass::onMouseMove
//

void OtApplicationClass::onMouseMove(int button, double xpos, double ypos) {
	// see if IMGUI wants the mouse event
	if (!mouseIMGUI()) {
		screen->onMouseMove(button, xpos, ypos);
	}
}


//
//	OtApplicationClass::onKey
//

void OtApplicationClass::onKey(int key, int scancode, int action, int mods) {
	// see if IMGUI wants the keyboard event
	if (!keyboardIMGUI() && action != GLFW_RELEASE) {
		screen->onKey(key, mods);
	}
}


//
//	OtApplicationClass::onChar
//

void OtApplicationClass::onChar(unsigned int codepoint) {
	// see if IMGUI wants the keyboard event
	if (!keyboardIMGUI()) {
		screen->onChar(codepoint);
	}
}


//
//	OtApplicationClass::getTime()
//

double OtApplicationClass::getTime() {
	return loopTime;
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
