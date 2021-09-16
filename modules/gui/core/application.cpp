//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <bx/timer.h>

#include "ot/function.h"
#include "ot/libuv.h"
#include "ot/vm.h"

#include "application.h"
#include "theme.h"


//
//	Globals
//

int OtApplicationClass::width = 1280;
int OtApplicationClass::height = 720;

size_t OtApplicationClass::frameNumber = 0;


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
		frameNumber++;
		timeGLFW();

		// update all animations
		for (int c = animations.size() - 1; c >= 0; c--) {
			if (!animations[c]->step(loopDuration * 1000)) {
				animations.erase(animations.begin() + c);
			}
		}

		// update all simulations
		for (auto& simulation : simulations) {
			if (simulation->isRunning()) {
				simulation->step(loopDuration * 1000);
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

	// remove all animations and simulations
	animations.clear();
	simulations.clear();

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
//	OtApplicationClass::profile
//

void OtApplicationClass::profile(bool profilerState) {
	profiler = profilerState;
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

	// add profiler (if required)
	if (profiler) {
		const bgfx::Stats* stats = bgfx::getStats();
		const double toMs = 1000.0 / double(bx::getHPFrequency());

		ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
		ImGui::Begin("Profiler", nullptr, 0);
		ImGui::Text("Framerate:"); ImGui::SameLine(150); ImGui::Text("%.1f", getFrameRate());
		ImGui::Text("GPU [ms]:"); ImGui::SameLine(150); ImGui::Text("%0.1f", double(stats->gpuTimeEnd - stats->gpuTimeBegin) * 1000.0 / stats->gpuTimerFreq);
		ImGui::Text("Backbuffer width:"); ImGui::SameLine(150); ImGui::Text("%d", stats->width);
		ImGui::Text("Backbuffer height:"); ImGui::SameLine(150); ImGui::Text("%d", stats->height);
		ImGui::Text("Draw calls:"); ImGui::SameLine(150); ImGui::Text("%d", stats->numDraw);
		ImGui::Text("Programs:"); ImGui::SameLine(150); ImGui::Text("%d", stats->numPrograms);
		ImGui::Text("Shaders:"); ImGui::SameLine(150); ImGui::Text("%d", stats->numShaders);
		ImGui::Text("Textures:"); ImGui::SameLine(150); ImGui::Text("%d", stats->numTextures);
		ImGui::Text("Uniforms:"); ImGui::SameLine(150); ImGui::Text("%d", stats->numUniforms);
		ImGui::Text("Vertex buffers:"); ImGui::SameLine(150); ImGui::Text("%d", stats->numVertexBuffers);
		ImGui::Text("Index buffers:"); ImGui::SameLine(150); ImGui::Text("%d", stats->numIndexBuffers);
		ImGui::End();
	}

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
		type->set("addSimulation", OtFunctionClass::create(&OtApplicationClass::addSimulation));
		type->set("profile", OtFunctionClass::create(&OtApplicationClass::profile));
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
