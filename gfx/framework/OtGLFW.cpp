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
		OtExcept(description);
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


//
//	OtFramework::addEnumsGLFW
//

void OtFramework::addEnumsGLFW(OtObject module) {
	module->set("mouseLeft", OtInteger::create(GLFW_MOUSE_BUTTON_LEFT));
	module->set("mouseRight", OtInteger::create(GLFW_MOUSE_BUTTON_RIGHT));
	module->set("mouseMiddle", OtInteger::create(GLFW_MOUSE_BUTTON_MIDDLE));

	module->set("keySpace", OtInteger::create(GLFW_KEY_SPACE));
	module->set("keyApostrophe", OtInteger::create(GLFW_KEY_APOSTROPHE));
	module->set("keyComma", OtInteger::create(GLFW_KEY_COMMA));
	module->set("keyMinus", OtInteger::create(GLFW_KEY_MINUS));
	module->set("keyPeriod", OtInteger::create(GLFW_KEY_PERIOD));
	module->set("keySlash", OtInteger::create(GLFW_KEY_SLASH));
	module->set("key0", OtInteger::create(GLFW_KEY_0));
	module->set("key1", OtInteger::create(GLFW_KEY_1));
	module->set("key2", OtInteger::create(GLFW_KEY_2));
	module->set("key3", OtInteger::create(GLFW_KEY_3));
	module->set("key4", OtInteger::create(GLFW_KEY_4));
	module->set("key5", OtInteger::create(GLFW_KEY_5));
	module->set("key6", OtInteger::create(GLFW_KEY_6));
	module->set("key7", OtInteger::create(GLFW_KEY_7));
	module->set("key8", OtInteger::create(GLFW_KEY_8));
	module->set("key9", OtInteger::create(GLFW_KEY_9));
	module->set("keySemiColon", OtInteger::create(GLFW_KEY_SEMICOLON));
	module->set("keyEqual", OtInteger::create(GLFW_KEY_EQUAL));
	module->set("keyA", OtInteger::create(GLFW_KEY_A));
	module->set("keyB", OtInteger::create(GLFW_KEY_B));
	module->set("keyC", OtInteger::create(GLFW_KEY_C));
	module->set("keyD", OtInteger::create(GLFW_KEY_D));
	module->set("keyE", OtInteger::create(GLFW_KEY_E));
	module->set("keyF", OtInteger::create(GLFW_KEY_F));
	module->set("keyG", OtInteger::create(GLFW_KEY_G));
	module->set("keyH", OtInteger::create(GLFW_KEY_H));
	module->set("keyI", OtInteger::create(GLFW_KEY_I));
	module->set("keyJ", OtInteger::create(GLFW_KEY_J));
	module->set("KeyPad", OtInteger::create(GLFW_KEY_K));
	module->set("keyL", OtInteger::create(GLFW_KEY_L));
	module->set("keyM", OtInteger::create(GLFW_KEY_M));
	module->set("keyN", OtInteger::create(GLFW_KEY_N));
	module->set("keyO", OtInteger::create(GLFW_KEY_O));
	module->set("keyP", OtInteger::create(GLFW_KEY_P));
	module->set("keyQ", OtInteger::create(GLFW_KEY_Q));
	module->set("keyR", OtInteger::create(GLFW_KEY_R));
	module->set("keyS", OtInteger::create(GLFW_KEY_S));
	module->set("keyT", OtInteger::create(GLFW_KEY_T));
	module->set("keyU", OtInteger::create(GLFW_KEY_U));
	module->set("keyV", OtInteger::create(GLFW_KEY_V));
	module->set("keyW", OtInteger::create(GLFW_KEY_W));
	module->set("keyX", OtInteger::create(GLFW_KEY_X));
	module->set("keyY", OtInteger::create(GLFW_KEY_Y));
	module->set("keyZ", OtInteger::create(GLFW_KEY_Z));
	module->set("keyLeftBracket", OtInteger::create(GLFW_KEY_LEFT_BRACKET));
	module->set("keyBackslash", OtInteger::create(GLFW_KEY_BACKSLASH));
	module->set("keyRightBracket", OtInteger::create(GLFW_KEY_RIGHT_BRACKET));
	module->set("keyGraveAccent", OtInteger::create(GLFW_KEY_GRAVE_ACCENT));

	module->set("keyEscape", OtInteger::create(GLFW_KEY_ESCAPE));
	module->set("keyEnter", OtInteger::create(GLFW_KEY_ENTER));
	module->set("keyTab", OtInteger::create(GLFW_KEY_TAB));
	module->set("keyBackspace", OtInteger::create(GLFW_KEY_BACKSPACE));
	module->set("keyInsert", OtInteger::create(GLFW_KEY_INSERT));
	module->set("keyDelete", OtInteger::create(GLFW_KEY_DELETE));
	module->set("keyRight", OtInteger::create(GLFW_KEY_RIGHT));
	module->set("keyLeft", OtInteger::create(GLFW_KEY_LEFT));
	module->set("keyDown", OtInteger::create(GLFW_KEY_DOWN));
	module->set("keyUp", OtInteger::create(GLFW_KEY_UP));
	module->set("keyPageUp", OtInteger::create(GLFW_KEY_PAGE_UP));
	module->set("keyPageDown", OtInteger::create(GLFW_KEY_PAGE_DOWN));
	module->set("keyHome", OtInteger::create(GLFW_KEY_HOME));
	module->set("keyEnd", OtInteger::create(GLFW_KEY_END));
	module->set("keyCapsLock", OtInteger::create(GLFW_KEY_CAPS_LOCK));
	module->set("keyScrollLock", OtInteger::create(GLFW_KEY_SCROLL_LOCK));
	module->set("keyNumLock", OtInteger::create(GLFW_KEY_NUM_LOCK));
	module->set("keyPrintScreen", OtInteger::create(GLFW_KEY_PRINT_SCREEN));
	module->set("keyPause", OtInteger::create(GLFW_KEY_PAUSE));
	module->set("keyF1", OtInteger::create(GLFW_KEY_F1));
	module->set("keyF2", OtInteger::create(GLFW_KEY_F2));
	module->set("keyF3", OtInteger::create(GLFW_KEY_F3));
	module->set("keyF4", OtInteger::create(GLFW_KEY_F4));
	module->set("keyF5", OtInteger::create(GLFW_KEY_F5));
	module->set("keyF6", OtInteger::create(GLFW_KEY_F6));
	module->set("keyF7", OtInteger::create(GLFW_KEY_F7));
	module->set("keyF8", OtInteger::create(GLFW_KEY_F8));
	module->set("keyF9", OtInteger::create(GLFW_KEY_F9));
	module->set("keyF10", OtInteger::create(GLFW_KEY_F10));
	module->set("keyF11", OtInteger::create(GLFW_KEY_F11));
	module->set("keyF12", OtInteger::create(GLFW_KEY_F12));
	module->set("KeyPadP0", OtInteger::create(GLFW_KEY_KP_0));
	module->set("KeyPadP1", OtInteger::create(GLFW_KEY_KP_1));
	module->set("KeyPadP2", OtInteger::create(GLFW_KEY_KP_2));
	module->set("KeyPadP3", OtInteger::create(GLFW_KEY_KP_3));
	module->set("KeyPadP4", OtInteger::create(GLFW_KEY_KP_4));
	module->set("KeyPadP5", OtInteger::create(GLFW_KEY_KP_5));
	module->set("KeyPadP6", OtInteger::create(GLFW_KEY_KP_6));
	module->set("KeyPadP7", OtInteger::create(GLFW_KEY_KP_7));
	module->set("KeyPadP8", OtInteger::create(GLFW_KEY_KP_8));
	module->set("KeyPadP9", OtInteger::create(GLFW_KEY_KP_9));
	module->set("KeyPadDecimal", OtInteger::create(GLFW_KEY_KP_DECIMAL));
	module->set("KeyPadDivide", OtInteger::create(GLFW_KEY_KP_DIVIDE));
	module->set("KeyPadMultiply", OtInteger::create(GLFW_KEY_KP_MULTIPLY));
	module->set("KeyPadSubtract", OtInteger::create(GLFW_KEY_KP_SUBTRACT));
	module->set("KeyPadAdd", OtInteger::create(GLFW_KEY_KP_ADD));
	module->set("KeyPadEnter", OtInteger::create(GLFW_KEY_KP_ENTER));
	module->set("KeyPadEqual", OtInteger::create(GLFW_KEY_KP_EQUAL));
	module->set("keyLeftShift", OtInteger::create(GLFW_KEY_LEFT_SHIFT));
	module->set("keyLeftControl", OtInteger::create(GLFW_KEY_LEFT_CONTROL));
	module->set("keyLeftAlt", OtInteger::create(GLFW_KEY_LEFT_ALT));
	module->set("keyLeftSuper", OtInteger::create(GLFW_KEY_LEFT_SUPER));
	module->set("keyRightShift", OtInteger::create(GLFW_KEY_RIGHT_SHIFT));
	module->set("keyRightControl", OtInteger::create(GLFW_KEY_RIGHT_CONTROL));
	module->set("keyRightAlt", OtInteger::create(GLFW_KEY_RIGHT_ALT));
	module->set("keyRightSuper", OtInteger::create(GLFW_KEY_RIGHT_SUPER));
	module->set("keyMenu", OtInteger::create(GLFW_KEY_MENU));

	module->set("keyModShift", OtInteger::create(GLFW_MOD_SHIFT));
	module->set("keyModCtrl", OtInteger::create(GLFW_MOD_CONTROL));
	module->set("keyModAlt", OtInteger::create(GLFW_MOD_ALT));
	module->set("keyModSuper", OtInteger::create(GLFW_MOD_SUPER));
	module->set("keyModCapsLock", OtInteger::create(GLFW_MOD_CAPS_LOCK));
	module->set("keyModNumLock", OtInteger::create(GLFW_MOD_NUM_LOCK));
}
