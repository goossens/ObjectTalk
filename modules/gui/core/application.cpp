//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <map>

#include "ot/function.h"
#include "ot/libuv.h"
#include "ot/vm.h"

#include "application.h"


//
//	OtAnimation
//

class OtAnimation
{
public:
	OtAnimation(double* v, double d, double l, double s, double e) : value(v), delay(d), length(l), start(s), end(e) {
		startTime = OtApplicationClass::getTime();
	}

	bool update(double delta) {
		// wait until we are at the end of the delay time
		bool done = false;

		if (delta >= delay) {
			// determine place in animation process
			delta = delta - delay;

			// have we reach the end of our animation?
			if (delta >= length) {
				// yes, we are done
				*value = end;
				done = true;

			} else {
				// no, calculate new animation value
				*value = start + (delta / length) * (end - start);
			}
		}

		return done;
	}

private:
	double startTime;
	double* value;
	double delay, length;
	double start, end;
};

static std::map<void*, std::shared_ptr<OtAnimation>> animations;


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
		OtVM::memberFunction(getSharedPtr(), "setup", screen);
	}

	// application main loop
	while (!glfwWindowShouldClose(window)) {
		// update all animations
		auto i = animations.begin();

		while (i != animations.end()) {
			if (i->second->update(loopDuration)) {
				animations.erase(i++);

			} else {
				i++;
			}
		}

		// call app's update member (if defined)
		if (has("update")) {
			OtVM::memberFunction(getSharedPtr(), "update");
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
		OtVM::memberFunction(getSharedPtr(), "terminate");
	}

	// remove all children from the screen to avoid memory leaks
	// it has circular parent/child relationships

	screen->clear();
	screen = nullptr;
	unsetAll();

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
//	OtApplicationClass::render
//

void OtApplicationClass::render() {
	// "run" all libraries
	frameGLFW();
	frameBGFX();
	frameIMGUI();

	// update an render all widgets
	screen->update();
	screen->render();

	// put results on screen
	renderIMGUI();
	renderBGFX();
}


//
//	OtApplicationClass::animate
//

void OtApplicationClass::animate(double* value, double delay, double length, double start, double end) {
	animations[value] = std::make_shared<OtAnimation>(value, delay, length, start, end);
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
	}

	return type;
}


//
//	OtApplicationClass::create
//

OtApplication OtApplicationClass::create() {
	OtApplication app = std::make_shared<OtApplicationClass>();
	app->setType(getMeta());
	return app;
}
