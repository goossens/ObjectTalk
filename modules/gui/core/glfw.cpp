//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/exception.h"

#include "application.h"


//
//	Globals
//

double OtApplicationClass::lastTime;
double OtApplicationClass::loopTime;
double OtApplicationClass::loopDuration;


//
//	OtApplicationClass::initGLFW
//

void OtApplicationClass::initGLFW(const std::string& name) {
	// initialize GLFW library
	glfwSetErrorCallback([](int error, const char* description) {
		OtExcept(description);
	});

	glfwInit();

	// create a new window
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	window = glfwCreateWindow(OtTheme::width, OtTheme::height, name.c_str(), NULL, NULL);
	glfwSetWindowUserPointer(window, this);

#if __APPLE__
	fixMenuLabels(name);
#endif

	// setup window resize callback
	glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int width, int height) {
		OtApplicationClass* app = (OtApplicationClass*) glfwGetWindowUserPointer(window);
		app->render();
	});

	// setup mouse button callback
	glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
		OtApplicationClass* app = (OtApplicationClass*) glfwGetWindowUserPointer(window);
		ImGuiIO& io = ImGui::GetIO();

		if (!io.WantCaptureMouse) {
			OT_DEBUG(button);
		}
	});

	// set keyboard callback
	glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
		OtApplicationClass* app = (OtApplicationClass*) glfwGetWindowUserPointer(window);
		ImGuiIO& io = ImGui::GetIO();

		if (!io.WantCaptureKeyboard) {
			OT_DEBUG(key);
		}
	});

	// start time tracking
	lastTime = glfwGetTime();
}


//
//	OtApplicationClass::frameGLFW
//

void OtApplicationClass::frameGLFW() {
	// get window size
	glfwGetFramebufferSize(window, &OtTheme::width, &OtTheme::height);

	// get time since epoch
	loopTime = glfwGetTime();
	loopDuration = loopTime - lastTime;
	lastTime = loopTime;
}


//
//	OtApplicationClass::eventsGLFW
//

void OtApplicationClass::eventsGLFW() {
	// poll for more windows events
	glfwPollEvents();
}


//
//	OtApplicationClass::endGLFW
//

void OtApplicationClass::endGLFW() {
	// terminate GLFW
	glfwDestroyWindow(window);
	glfwTerminate();
}
