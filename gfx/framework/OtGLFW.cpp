//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
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
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#undef APIENTRY

#else
#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_GLX
#endif

#include "GLFW/glfw3native.h"

#include "OtException.h"
#include "OtFormat.h"
#include "OtFunction.h"
#include "OtInteger.h"

#include "OtFramework.h"


//
//	OtFramework::initGLFW
//

void OtFramework::initGLFW() {
	// initialize GLFW library
	glfwSetErrorCallback([](int error, const char* description) {
		OtError(description);
	});

	glfwInit();

	// determine best default window size
	int x, y, w, h;
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	glfwGetMonitorWorkarea(monitor, &x, &y, &w, &h);

	if (w >= 1600 && h >= 900) {
		width = 1600;
		height = 900;

	} else if (w >= 1440 && h >= 810) {
		width = 1440;
		height = 810;

	} else if (w >= 1280 && h >= 720) {
		width = 1280;
		height = 720;

	} else if (w >= 1024 && h >= 576) {
		width = 1024;
		height = 576;

	} else if (w >= 800 && h >= 450) {
		width = 800;
		height = 450;

	} else {
		width = 640;
		height = 360;
	}

	// create a new window
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	window = glfwCreateWindow(width, height, "ObjectTalk", NULL, NULL);
	glfwSetWindowAspectRatio(window, 16, 9);
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
	fixMenuLabels();
#endif

	// setup window close callback
	glfwSetWindowCloseCallback(window, [](GLFWwindow* window) {
		OtFramework* fw = (OtFramework*) glfwGetWindowUserPointer(window);

		// call close callbacks
		if (!fw->canQuit()) {
			glfwSetWindowShouldClose(window, GLFW_FALSE);
		}
	});

	// setup window resize callback
	glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int w, int h) {
		OtFramework* fw = (OtFramework*) glfwGetWindowUserPointer(window);
		fw->width = w;
		fw->height = h;
	});

	// setup window focus callback
	glfwSetWindowFocusCallback(window, [](GLFWwindow* window, int focused) {
		OtFramework* fw = (OtFramework*) glfwGetWindowUserPointer(window);

		if (focused) {
			double xpos, ypos;
			glfwGetCursorPos(window, &xpos, &ypos);
			fw->eventQueue.pushMouseMoveEvent(xpos, ypos);
		}
	});

	// setup mouse button callback
	glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
		OtFramework* fw = (OtFramework*) glfwGetWindowUserPointer(window);

		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		fw->modifiers = (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS) ? mods : 0;
		fw->eventQueue.pushMouseButtonEvent(button, action, mods, xpos, ypos);
	});

	// setup mouse move callback
	glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
		OtFramework* fw = (OtFramework*) glfwGetWindowUserPointer(window);

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
			fw->eventQueue.pushMouseDragEvent(GLFW_MOUSE_BUTTON_1, fw->modifiers, xpos, ypos);

		} else {
			fw->eventQueue.pushMouseMoveEvent(xpos, ypos);
		}
	});

	// setup scroll wheel callback
	glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset) {
		OtFramework* fw = (OtFramework*) glfwGetWindowUserPointer(window);
		fw->eventQueue.pushMouseWheelEvent(xoffset, yoffset);
	});

	// set keyboard callback
	glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
		OtFramework* fw = (OtFramework*) glfwGetWindowUserPointer(window);

		if (key == GLFW_KEY_F5 && action == GLFW_PRESS) {
			fw->profiler = !fw->profiler;

		} else if (key == GLFW_KEY_F6 && action == GLFW_PRESS) {
			fw->demo = !fw->demo;

#if !__APPLE__
		} else if ((mods & GLFW_MOD_CONTROL) && key == GLFW_KEY_Q && action == GLFW_PRESS) {
			if (fw->canQuit()) {
				fw->stopGLFW();
			}

#endif

		} else {
			fw->eventQueue.pushKeyboardEvent(key, scancode, action, mods);
		}
	});

	// set character callback
	glfwSetCharCallback(window, [](GLFWwindow* window, unsigned int codepoint) {
		OtFramework* fw = (OtFramework*) glfwGetWindowUserPointer(window);
		fw->eventQueue.pushCharacterEvent(codepoint);
	});

	// capture mouse starting point
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	eventQueue.pushMouseMoveEvent(xpos, ypos);
}


//
//	OtFramework::runningGLFW
//

bool OtFramework::runningGLFW() {
	return !glfwWindowShouldClose(window);
}


//
//	OtFramework::eventsGLFW
//

void OtFramework::eventsGLFW() {
	// wait for window events
	glfwWaitEventsTimeout(0.016);

	// poll gamepad events
	for (auto gamepad = 0; gamepad < 4; gamepad++) {
		int numAxes;
		const float* axes = glfwGetJoystickAxes(gamepad, &numAxes);

		if (axes) {
			for (auto axis = 0; axis < std::min(numAxes, 2); axis++) {
				eventQueue.pushGamepadAxisEvent(gamepad, axis, std::round(axes[axis] * 100.0));
			}
		}

		int numButtons;
		const unsigned char* buttons = glfwGetJoystickButtons(gamepad, &numButtons);

		if (buttons) {
			for (auto button = 0; button < std::min(numButtons, 8); button++) {
				if (buttons[button] != gamepads[gamepad].buttons[button]) {
					gamepads[gamepad].buttons[button] = buttons[button];
					eventQueue.pushGamepadButtonEvent(gamepad, button, buttons[button]);
				}
			}
		}
	}
}


//
//	OtFramework::stopGLFW
//

void OtFramework::stopGLFW() {
	glfwSetWindowShouldClose(window, GLFW_TRUE);
}


//
//	OtFramework::endGLFW
//

void OtFramework::endGLFW() {
	// terminate GLFW
	glfwDestroyWindow(window);
	glfwTerminate();
}
