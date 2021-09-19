//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#if __APPLE__
#define GLFW_EXPOSE_NATIVE_COCOA

#elif defined(_WIN32)
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL

#else
#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_GLX
#endif

#include "GLFW/glfw3native.h"

#include "ot/exception.h"
#include "ot/format.h"
#include "ot/function.h"

#include "application.h"
#include "theme.h"


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
	window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
	glfwSetWindowUserPointer(window, this);

	// get native handles
#if __APPLE__
	nativeDisplayHandle = glfwGetCocoaWindow(window);
	createMetalLayer();

#elif defined(_WIN32)
	nativeDisplayHandle = glfwGetWin32Window(window);

#else
	nativeDisplayHandle = (void*) glfwGetX11Window(window);
	nativeDisplayType = glfwGetX11Display();
#endif

#if __APPLE__
	fixMenuLabels(name);
#endif

	// setup window resize callback
	glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int w, int h) {
		OtApplicationClass* app = (OtApplicationClass*) glfwGetWindowUserPointer(window);
		width = w;
		height = h;
	});

	// setup mouse button callback
	glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
		OtApplicationClass* app = (OtApplicationClass*) glfwGetWindowUserPointer(window);
		app->pushEvent(OtFormat("click %d %d %d", button, action, mods));
	});

	// setup mouse move callback
	glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
		OtApplicationClass* app = (OtApplicationClass*) glfwGetWindowUserPointer(window);
		app->pushEvent(OtFormat("move %.3f %.3f", xpos, ypos));
	});

	// setup scroll wheel callback
	glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset) {
		OtApplicationClass* app = (OtApplicationClass*) glfwGetWindowUserPointer(window);
		app->pushEvent(OtFormat("wheel %.3f %.3f", xoffset, yoffset));
	});

	// set keyboard callback
	glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
		OtApplicationClass* app = (OtApplicationClass*) glfwGetWindowUserPointer(window);

		if (key == GLFW_KEY_F5 && action == GLFW_PRESS) {
			app->profiler = !app->profiler;

		} else {
			app->pushEvent(OtFormat("key %d %d %d %d", key, scancode, action, mods));
		}
	});

	// set character callback
	glfwSetCharCallback(window, [](GLFWwindow* window, unsigned int codepoint) {
		OtApplicationClass* app = (OtApplicationClass*) glfwGetWindowUserPointer(window);
		app->pushEvent(OtFormat("char %d", codepoint));
	});

	// capture mouse starting point
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	pushEvent(OtFormat("move -1 %.3f %.3f", xpos, ypos));
}


//
//	OtApplicationClass::runningGLFW
//

bool OtApplicationClass::runningGLFW() {
	return !glfwWindowShouldClose(window);
}


//
//	OtApplicationClass::eventsGLFW
//

void OtApplicationClass::eventsGLFW() {
	// wait for window events
	glfwWaitEventsTimeout(0.016);
}


//
//	OtApplicationClass::endGLFW
//

void OtApplicationClass::endGLFW() {
	// terminate GLFW
	glfwDestroyWindow(window);
	glfwTerminate();
}
