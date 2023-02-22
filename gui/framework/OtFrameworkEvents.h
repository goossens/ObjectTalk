//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtConcurrentQueue.h"

#include "OtFrameworkApp.h"


//
//	Event definition
//

struct OtFwEvent {
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
			OtFrameworkApp* customer;
		} customerSetup;

		struct {
			OtFrameworkApp* customer;
		} customerTerminate;

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
		return type == OtFwEvent::mouseButtonEvent ||
			type == OtFwEvent::mouseMoveEvent ||
			type == OtFwEvent::mouseDragEvent ||
			type == OtFwEvent::mouseWheelEvent;
	}

	bool isKeyboardEvent() {
		return type == OtFwEvent::keyboardEvent ||
			type == OtFwEvent::characterEvent;
	}

	bool isGamepadEvent() {
		return type == gamepadAxisEvent ||
			type == gamepadButtonEvent;
	}
};


//
//	Thread-safe app event queue
//

class OtFwEventQueue : public OtConcurrentQueue<OtFwEvent> {
public:
	void pushMouseButtonEvent(int button, int action, int mods, float x, float y) {
		OtFwEvent event;
		event.type = OtFwEvent::mouseButtonEvent;
		event.mouseButton.button = button;
		event.mouseButton.action = action;
		event.mouseButton.mods = mods;
		event.mouseButton.x = x;
		event.mouseButton.y = y;
		push(event);
	}

	void pushMouseMoveEvent(float x, float y) {
		OtFwEvent event;
		event.type = OtFwEvent::mouseMoveEvent;
		event.mouseMove.x = x;
		event.mouseMove.y = y;
		push(event);
	}

	void pushMouseDragEvent(int button, int mods, float x, float y) {
		OtFwEvent event;
		event.type = OtFwEvent::mouseDragEvent;
		event.mouseDrag.button = button;
		event.mouseDrag.mods = mods;
		event.mouseDrag.x = x;
		event.mouseDrag.y = y;
		push(event);
	}

	void pushMouseWheelEvent(float xOffset, float yOffset) {
		OtFwEvent event;
		event.type = OtFwEvent::mouseWheelEvent;
		event.mouseWheel.xOffset = xOffset;
		event.mouseWheel.yOffset = yOffset;
		push(event);
	}

	void pushKeyboardEvent(int key, int scancode, int action, int mods) {
		OtFwEvent event;
		event.type = OtFwEvent::keyboardEvent;
		event.keyboard.key = key;
		event.keyboard.scancode = scancode;
		event.keyboard.action = action;
		event.keyboard.mods = mods;
		push(event);
	}

	void pushCharacterEvent(int codepoint) {
		OtFwEvent event;
		event.type = OtFwEvent::characterEvent;
		event.character.codepoint = codepoint;
		push(event);
	}

	void pushGamepadAxisEvent(int gamepad, int axis, int value) {
		OtFwEvent event;
		event.type = OtFwEvent::gamepadAxisEvent;
		event.gamepadAxis.gamepad = gamepad;
		event.gamepadAxis.axis = axis;
		event.gamepadAxis.value = value;
		push(event);
	}

	void pushGamepadButtonEvent(int gamepad, int button, int action) {
		OtFwEvent event;
		event.type = OtFwEvent::gamepadButtonEvent;
		event.gamepadButton.gamepad = gamepad;
		event.gamepadButton.button = button;
		event.gamepadButton.action = action;
		push(event);
	}
};
