//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
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

#else
#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_GLX
#endif

#include "GLFW/glfw3native.h"

#include "ot/exception.h"
#include "ot/format.h"
#include "ot/function.h"
#include "ot/integer.h"

#include "application.h"


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
	fixMenuLabels(name);
#endif

	// setup window close callback
	glfwSetWindowCloseCallback(window, [](GLFWwindow* window) {
		OtApplicationClass* app = (OtApplicationClass*) glfwGetWindowUserPointer(window);

		// call close callbacks
		for (auto& callback : app->atCloseCallbacks) {
			if (!callback()) {
				glfwSetWindowShouldClose(window, GLFW_FALSE);
			}
		}
	});

	// setup window resize callback
	glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int w, int h) {
		OtApplicationClass* app = (OtApplicationClass*) glfwGetWindowUserPointer(window);
		app->width = w;
		app->height = h;
	});

	// setup mouse button callback
	glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
		OtApplicationClass* app = (OtApplicationClass*) glfwGetWindowUserPointer(window);

		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		app->modifiers = (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS) ? mods : 0;

		app->eventQueue.pushMouseButtonEvent(button, action, mods, xpos, ypos);
	});

	// setup mouse move callback
	glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
		OtApplicationClass* app = (OtApplicationClass*) glfwGetWindowUserPointer(window);
		int button = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1);

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
			app->eventQueue.pushMouseDragEvent(GLFW_MOUSE_BUTTON_1, app->modifiers, xpos, ypos);

		} else {
			app->eventQueue.pushMouseMoveEvent(xpos, ypos);
		}
	});

	// setup scroll wheel callback
	glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset) {
		OtApplicationClass* app = (OtApplicationClass*) glfwGetWindowUserPointer(window);
		app->eventQueue.pushMouseWheelEvent(xoffset, yoffset);
	});

	// set keyboard callback
	glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
		OtApplicationClass* app = (OtApplicationClass*) glfwGetWindowUserPointer(window);

		if (key == GLFW_KEY_F5 && action == GLFW_PRESS) {
			app->profiler = !app->profiler;

		} else {
			app->eventQueue.pushKeyboardEvent(key, scancode, action, mods);
		}
	});

	// set character callback
	glfwSetCharCallback(window, [](GLFWwindow* window, unsigned int codepoint) {
		OtApplicationClass* app = (OtApplicationClass*) glfwGetWindowUserPointer(window);
		app->eventQueue.pushCharacterEvent(codepoint);
	});

	// capture mouse starting point
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	eventQueue.pushMouseMoveEvent(xpos, ypos);
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
//	OtApplicationClass::quitGLFW
//

void OtApplicationClass::quitGLFW() {
	glfwSetWindowShouldClose(window, GLFW_TRUE);
}


//
//	OtApplicationClass::endGLFW
//

void OtApplicationClass::endGLFW() {
	// terminate GLFW
	glfwDestroyWindow(window);
	glfwTerminate();
}


//
//	OtApplicationClass::addEnumsGLFW
//

void OtApplicationClass::addEnumsGLFW(OtObject module) {
	module->set("mouseLeft", OtIntegerClass::create(GLFW_MOUSE_BUTTON_LEFT));
	module->set("mouseRight", OtIntegerClass::create(GLFW_MOUSE_BUTTON_RIGHT));
	module->set("mouseMiddle", OtIntegerClass::create(GLFW_MOUSE_BUTTON_MIDDLE));

	module->set("keySpace", OtIntegerClass::create(GLFW_KEY_SPACE));
	module->set("keyApostrophe", OtIntegerClass::create(GLFW_KEY_APOSTROPHE));
	module->set("keyComma", OtIntegerClass::create(GLFW_KEY_COMMA));
	module->set("keyMinus", OtIntegerClass::create(GLFW_KEY_MINUS));
	module->set("keyPeriod", OtIntegerClass::create(GLFW_KEY_PERIOD));
	module->set("keySlash", OtIntegerClass::create(GLFW_KEY_SLASH));
	module->set("key0", OtIntegerClass::create(GLFW_KEY_0));
	module->set("key1", OtIntegerClass::create(GLFW_KEY_1));
	module->set("key2", OtIntegerClass::create(GLFW_KEY_2));
	module->set("key3", OtIntegerClass::create(GLFW_KEY_3));
	module->set("key4", OtIntegerClass::create(GLFW_KEY_4));
	module->set("key5", OtIntegerClass::create(GLFW_KEY_5));
	module->set("key6", OtIntegerClass::create(GLFW_KEY_6));
	module->set("key7", OtIntegerClass::create(GLFW_KEY_7));
	module->set("key8", OtIntegerClass::create(GLFW_KEY_8));
	module->set("key9", OtIntegerClass::create(GLFW_KEY_9));
	module->set("keySemiColon", OtIntegerClass::create(GLFW_KEY_SEMICOLON));
	module->set("keyEqual", OtIntegerClass::create(GLFW_KEY_EQUAL));
	module->set("keyA", OtIntegerClass::create(GLFW_KEY_A));
	module->set("keyB", OtIntegerClass::create(GLFW_KEY_B));
	module->set("keyC", OtIntegerClass::create(GLFW_KEY_C));
	module->set("keyD", OtIntegerClass::create(GLFW_KEY_D));
	module->set("keyE", OtIntegerClass::create(GLFW_KEY_E));
	module->set("keyF", OtIntegerClass::create(GLFW_KEY_F));
	module->set("keyG", OtIntegerClass::create(GLFW_KEY_G));
	module->set("keyH", OtIntegerClass::create(GLFW_KEY_H));
	module->set("keyI", OtIntegerClass::create(GLFW_KEY_I));
	module->set("keyJ", OtIntegerClass::create(GLFW_KEY_J));
	module->set("KeyPad", OtIntegerClass::create(GLFW_KEY_K));
	module->set("keyL", OtIntegerClass::create(GLFW_KEY_L));
	module->set("keyM", OtIntegerClass::create(GLFW_KEY_M));
	module->set("keyN", OtIntegerClass::create(GLFW_KEY_N));
	module->set("keyO", OtIntegerClass::create(GLFW_KEY_O));
	module->set("keyP", OtIntegerClass::create(GLFW_KEY_P));
	module->set("keyQ", OtIntegerClass::create(GLFW_KEY_Q));
	module->set("keyR", OtIntegerClass::create(GLFW_KEY_R));
	module->set("keyS", OtIntegerClass::create(GLFW_KEY_S));
	module->set("keyT", OtIntegerClass::create(GLFW_KEY_T));
	module->set("keyU", OtIntegerClass::create(GLFW_KEY_U));
	module->set("keyV", OtIntegerClass::create(GLFW_KEY_V));
	module->set("keyW", OtIntegerClass::create(GLFW_KEY_W));
	module->set("keyX", OtIntegerClass::create(GLFW_KEY_X));
	module->set("keyY", OtIntegerClass::create(GLFW_KEY_Y));
	module->set("keyZ", OtIntegerClass::create(GLFW_KEY_Z));
	module->set("keyLeftBracket", OtIntegerClass::create(GLFW_KEY_LEFT_BRACKET));
	module->set("keyBackslash", OtIntegerClass::create(GLFW_KEY_BACKSLASH));
	module->set("keyRightBracket", OtIntegerClass::create(GLFW_KEY_RIGHT_BRACKET));
	module->set("keyGraveAccent", OtIntegerClass::create(GLFW_KEY_GRAVE_ACCENT));

	module->set("keyEscape", OtIntegerClass::create(GLFW_KEY_ESCAPE));
	module->set("keyEnter", OtIntegerClass::create(GLFW_KEY_ENTER));
	module->set("keyTab", OtIntegerClass::create(GLFW_KEY_TAB));
	module->set("keyBackspace", OtIntegerClass::create(GLFW_KEY_BACKSPACE));
	module->set("keyInsert", OtIntegerClass::create(GLFW_KEY_INSERT));
	module->set("keyDelete", OtIntegerClass::create(GLFW_KEY_DELETE));
	module->set("keyRight", OtIntegerClass::create(GLFW_KEY_RIGHT));
	module->set("keyLeft", OtIntegerClass::create(GLFW_KEY_LEFT));
	module->set("keyDown", OtIntegerClass::create(GLFW_KEY_DOWN));
	module->set("keyUp", OtIntegerClass::create(GLFW_KEY_UP));
	module->set("keyPageUp", OtIntegerClass::create(GLFW_KEY_PAGE_UP));
	module->set("keyPageDown", OtIntegerClass::create(GLFW_KEY_PAGE_DOWN));
	module->set("keyHome", OtIntegerClass::create(GLFW_KEY_HOME));
	module->set("keyEnd", OtIntegerClass::create(GLFW_KEY_END));
	module->set("keyCapsLock", OtIntegerClass::create(GLFW_KEY_CAPS_LOCK));
	module->set("keyScrollLock", OtIntegerClass::create(GLFW_KEY_SCROLL_LOCK));
	module->set("keyNumLock", OtIntegerClass::create(GLFW_KEY_NUM_LOCK));
	module->set("keyPrintScreen", OtIntegerClass::create(GLFW_KEY_PRINT_SCREEN));
	module->set("keyPause", OtIntegerClass::create(GLFW_KEY_PAUSE));
	module->set("keyF1", OtIntegerClass::create(GLFW_KEY_F1));
	module->set("keyF2", OtIntegerClass::create(GLFW_KEY_F2));
	module->set("keyF3", OtIntegerClass::create(GLFW_KEY_F3));
	module->set("keyF4", OtIntegerClass::create(GLFW_KEY_F4));
	module->set("keyF5", OtIntegerClass::create(GLFW_KEY_F5));
	module->set("keyF6", OtIntegerClass::create(GLFW_KEY_F6));
	module->set("keyF7", OtIntegerClass::create(GLFW_KEY_F7));
	module->set("keyF8", OtIntegerClass::create(GLFW_KEY_F8));
	module->set("keyF9", OtIntegerClass::create(GLFW_KEY_F9));
	module->set("keyF10", OtIntegerClass::create(GLFW_KEY_F10));
	module->set("keyF11", OtIntegerClass::create(GLFW_KEY_F11));
	module->set("keyF12", OtIntegerClass::create(GLFW_KEY_F12));
	module->set("KeyPadP0", OtIntegerClass::create(GLFW_KEY_KP_0));
	module->set("KeyPadP1", OtIntegerClass::create(GLFW_KEY_KP_1));
	module->set("KeyPadP2", OtIntegerClass::create(GLFW_KEY_KP_2));
	module->set("KeyPadP3", OtIntegerClass::create(GLFW_KEY_KP_3));
	module->set("KeyPadP4", OtIntegerClass::create(GLFW_KEY_KP_4));
	module->set("KeyPadP5", OtIntegerClass::create(GLFW_KEY_KP_5));
	module->set("KeyPadP6", OtIntegerClass::create(GLFW_KEY_KP_6));
	module->set("KeyPadP7", OtIntegerClass::create(GLFW_KEY_KP_7));
	module->set("KeyPadP8", OtIntegerClass::create(GLFW_KEY_KP_8));
	module->set("KeyPadP9", OtIntegerClass::create(GLFW_KEY_KP_9));
	module->set("KeyPadDecimal", OtIntegerClass::create(GLFW_KEY_KP_DECIMAL));
	module->set("KeyPadDivide", OtIntegerClass::create(GLFW_KEY_KP_DIVIDE));
	module->set("KeyPadMultiply", OtIntegerClass::create(GLFW_KEY_KP_MULTIPLY));
	module->set("KeyPadSubtract", OtIntegerClass::create(GLFW_KEY_KP_SUBTRACT));
	module->set("KeyPadAdd", OtIntegerClass::create(GLFW_KEY_KP_ADD));
	module->set("KeyPadEnter", OtIntegerClass::create(GLFW_KEY_KP_ENTER));
	module->set("KeyPadEqual", OtIntegerClass::create(GLFW_KEY_KP_EQUAL));
	module->set("keyLeftShift", OtIntegerClass::create(GLFW_KEY_LEFT_SHIFT));
	module->set("keyLeftControl", OtIntegerClass::create(GLFW_KEY_LEFT_CONTROL));
	module->set("keyLeftAlt", OtIntegerClass::create(GLFW_KEY_LEFT_ALT));
	module->set("keyLeftSuper", OtIntegerClass::create(GLFW_KEY_LEFT_SUPER));
	module->set("keyRightShift", OtIntegerClass::create(GLFW_KEY_RIGHT_SHIFT));
	module->set("keyRightControl", OtIntegerClass::create(GLFW_KEY_RIGHT_CONTROL));
	module->set("keyRightAlt", OtIntegerClass::create(GLFW_KEY_RIGHT_ALT));
	module->set("keyRightSuper", OtIntegerClass::create(GLFW_KEY_RIGHT_SUPER));
	module->set("keyMenu", OtIntegerClass::create(GLFW_KEY_MENU));

	module->set("keyModShift", OtIntegerClass::create(GLFW_MOD_SHIFT));
	module->set("keyModCtrl", OtIntegerClass::create(GLFW_MOD_CONTROL));
	module->set("keyModAlt", OtIntegerClass::create(GLFW_MOD_ALT));
	module->set("keyModSuper", OtIntegerClass::create(GLFW_MOD_SUPER));
	module->set("keyModCapsLock", OtIntegerClass::create(GLFW_MOD_CAPS_LOCK));
	module->set("keyModNumLock", OtIntegerClass::create(GLFW_MOD_NUM_LOCK));
}
