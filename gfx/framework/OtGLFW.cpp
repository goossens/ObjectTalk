//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
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

#elif _WIN32
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

#include "OtFunction.h"
#include "OtInteger.h"
#include "OtLog.h"

#include "OtFramework.h"


//
//	keysToMod
//

#if !__APPLE__ && !_WIN32
static int keysToMods(GLFWwindow* window) {
	// keyboard modifiers are not handled correctly on Linux with X11
	// see https://github.com/glfw/glfw/issues/1630

	int mods = 0;

	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS) {
		mods |= GLFW_MOD_CONTROL;
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) {
		mods |= GLFW_MOD_SHIFT;
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_ALT) == GLFW_PRESS) {
		mods |= GLFW_MOD_ALT;
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT_SUPER) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SUPER) == GLFW_PRESS) {
		mods |= GLFW_MOD_SUPER;
	}

	return mods;
}
#endif


//
//	OtFramework::initGLFW
//

void OtFramework::initGLFW() {
	// initialize GLFW library
	glfwSetErrorCallback([](int error, const char* description) {
		OtLogFatal(description);
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

#elif _WIN32
	nativeDisplayHandle = glfwGetWin32Window(window);

#else
	nativeDisplayHandle = (void*) glfwGetX11Window(window);
	nativeDisplayType = glfwGetX11Display();
#endif

#if __APPLE__
	fixMenuLabels();
#endif

	// create cursors
	cursors[ImGuiMouseCursor_Arrow] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
	cursors[ImGuiMouseCursor_TextInput] = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
	cursors[ImGuiMouseCursor_ResizeNS] = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
	cursors[ImGuiMouseCursor_ResizeEW] = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
	cursors[ImGuiMouseCursor_Hand] = glfwCreateStandardCursor(GLFW_HAND_CURSOR);

#if __APPLE__ || _WIN32
	cursors[ImGuiMouseCursor_ResizeAll] = glfwCreateStandardCursor(GLFW_RESIZE_ALL_CURSOR);
	cursors[ImGuiMouseCursor_ResizeNESW] = glfwCreateStandardCursor(GLFW_RESIZE_NESW_CURSOR);
	cursors[ImGuiMouseCursor_ResizeNWSE] = glfwCreateStandardCursor(GLFW_RESIZE_NWSE_CURSOR);
	cursors[ImGuiMouseCursor_NotAllowed] = glfwCreateStandardCursor(GLFW_NOT_ALLOWED_CURSOR);
#else
	cursors[ImGuiMouseCursor_ResizeAll] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
	cursors[ImGuiMouseCursor_ResizeNESW] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
	cursors[ImGuiMouseCursor_ResizeNWSE] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
	cursors[ImGuiMouseCursor_NotAllowed] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
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
			fw->eventQueue.pushMouseMoveEvent((float) xpos, (float) ypos);
		}
	});

	// setup mouse button callback
	glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
		OtFramework* fw = (OtFramework*) glfwGetWindowUserPointer(window);

		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		fw->modifiers = (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS) ? mods : 0;

#if !__APPLE__ && !_WIN32
		mods = (action == GLFW_PRESS) ? mods | keysToMods(window) : mods & ~keysToMods(window);
#endif

		fw->eventQueue.pushMouseButtonEvent(button, action, mods, (float) xpos, (float) ypos);
	});

	// setup mouse move callback
	glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
		OtFramework* fw = (OtFramework*) glfwGetWindowUserPointer(window);

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
			fw->eventQueue.pushMouseDragEvent(GLFW_MOUSE_BUTTON_1, fw->modifiers, xpos, ypos);

		} else {
			fw->eventQueue.pushMouseMoveEvent((float) xpos, (float) ypos);
		}
	});

	// setup scroll wheel callback
	glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset) {
		OtFramework* fw = (OtFramework*) glfwGetWindowUserPointer(window);
		fw->eventQueue.pushMouseWheelEvent((float) xoffset, (float) yoffset);
	});

	// set keyboard callback
	glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
		OtFramework* fw = (OtFramework*) glfwGetWindowUserPointer(window);

		if (key == GLFW_KEY_F16 && action == GLFW_PRESS) {
			fw->profiler = !fw->profiler;

		} else if (key == GLFW_KEY_F17 && action == GLFW_PRESS) {
			fw->metrics = !fw->metrics;

		} else if (key == GLFW_KEY_F18 && action == GLFW_PRESS) {
			fw->demo = !fw->demo;

#if !__APPLE__
		} else if ((mods & GLFW_MOD_CONTROL) && key == GLFW_KEY_Q && action == GLFW_PRESS) {
			if (fw->canQuit()) {
				fw->stopGLFW();
			}

#endif

		} else {
#if !__APPLE__ && !_WIN32
			mods = (action == GLFW_PRESS) ? mods | keysToMods(window) : mods & ~keysToMods(window);
#endif

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
	glfwWaitEventsTimeout(0.01f);

	// update cursor (if required)
	if (setCursor) {
		if (cursor == ImGuiMouseCursor_None) {
			// hide OS mouse cursor if imgui is drawing it or if it wants no cursor
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		} else {
			// Show OS mouse cursor
			glfwSetCursor(window, cursors[cursor] ? cursors[cursor] : cursors[ImGuiMouseCursor_Arrow]);
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}

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
	// free cursors
	for (ImGuiMouseCursor c = 0; c < ImGuiMouseCursor_COUNT; c++) {
		glfwDestroyCursor(cursors[c]);
	}

	// terminate GLFW
	glfwDestroyWindow(window);
	glfwTerminate();
}
