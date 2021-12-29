//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "ot/concurrentqueue.h"


//
//	Event definition
//

struct OtAppEvent {
	enum {
		mouseButtonEvent,
		mouseMoveEvent,
		mouseDragEvent,
		mouseWheelEvent,
		keyboardEvent,
		characterEvent,
		gamepadAxisEvent,
		gamepadButtonEvent
	} type;

	union {
		struct {
			int button;
			int action;
			int mods;
			float x;
			float y;
		} mouseButton;

		struct {
			float x;
			float y;
		} mouseMove;

		struct {
			int button;
			int mods;
			float x;
			float y;
		} mouseDrag;

		struct {
			float xOffset;
			float yOffset;
		} mouseWheel;

		struct {
			int key;
			int scancode;
			int action;
			int mods;
		} keyboard;

		struct {
			int codepoint;
		} character;

		struct {
			int gamepad;
			int axis;
			int value;
		} gamepadAxis;

		struct {
			int gamepad;
			int button;
			int action;
		} gamepadButton;
	};

	bool isMouseEvent() {
		return type == OtAppEvent::mouseButtonEvent ||
			type == OtAppEvent::mouseMoveEvent ||
			type == OtAppEvent::mouseDragEvent ||
			type == OtAppEvent::mouseWheelEvent;
	}

	bool isKeyboardEvent() {
		return type == OtAppEvent::keyboardEvent ||
			type == OtAppEvent::characterEvent;
	}

	bool isGamepadEvent() {
		return type == gamepadAxisEvent ||
			type == gamepadButtonEvent;
	}
};


//
//	Thread-safe app event queue
//

class OtAppEventQueue : public OtConcurrentQueue<OtAppEvent> {
public:
	void pushMouseButtonEvent(int button, int action, int mods, float x, float y) {
		OtAppEvent event;
		event.type = OtAppEvent::mouseButtonEvent;
		event.mouseButton.button = button;
		event.mouseButton.action = action;
		event.mouseButton.mods = mods;
		event.mouseButton.x = x;
		event.mouseButton.y = y;
		push(event);
	}

	void pushMouseMoveEvent(float x, float y) {
		OtAppEvent event;
		event.type = OtAppEvent::mouseMoveEvent;
		event.mouseMove.x = x;
		event.mouseMove.y = y;
		push(event);
	}

	void pushMouseDragEvent(int button, int mods, float x, float y) {
		OtAppEvent event;
		event.type = OtAppEvent::mouseDragEvent;
		event.mouseDrag.button = button;
		event.mouseDrag.mods = mods;
		event.mouseDrag.x = x;
		event.mouseDrag.y = y;
		push(event);
	}

	void pushMouseWheelEvent(float xOffset, float yOffset) {
		OtAppEvent event;
		event.type = OtAppEvent::mouseWheelEvent;
		event.mouseWheel.xOffset = xOffset;
		event.mouseWheel.yOffset = yOffset;
		push(event);
	}

	void pushKeyboardEvent(int key, int scancode, int action, int mods) {
		OtAppEvent event;
		event.type = OtAppEvent::keyboardEvent;
		event.keyboard.key = key;
		event.keyboard.scancode = scancode;
		event.keyboard.action = action;
		event.keyboard.mods = mods;
		push(event);
	}

	void pushCharacterEvent(int codepoint) {
		OtAppEvent event;
		event.type = OtAppEvent::characterEvent;
		event.character.codepoint = codepoint;
		push(event);
	}

	void pushGamepadAxisEvent(int gamepad, int axis, int value) {
		OtAppEvent event;
		event.type = OtAppEvent::gamepadAxisEvent;
		event.gamepadAxis.gamepad = gamepad;
		event.gamepadAxis.axis = axis;
		event.gamepadAxis.value = value;
		push(event);
	}

	void pushGamepadButtonEvent(int gamepad, int button, int action) {
		OtAppEvent event;
		event.type = OtAppEvent::gamepadButtonEvent;
		event.gamepadButton.gamepad = gamepad;
		event.gamepadButton.button = button;
		event.gamepadButton.action = action;
		push(event);
	}
};
