//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
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
	enum class Type {
		mouseButton,
		mouseMove,
		mouseWheel,
		keyboard,
		character,
		gamepadAxis,
		gamepadButton
	};

	Type type;

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

	inline bool isMouseEvent() {
		return type == Type::mouseButton ||
			type == Type::mouseMove ||
			type == Type::mouseWheel;
	}

	inline bool isKeyboardEvent() {
		return type == Type::keyboard ||
			type == Type::character;
	}

	inline bool isGamepadEvent() {
		return type == Type::gamepadAxis ||
			type == Type::gamepadButton;
	}
};


//
//	Thread-safe app event queue
//

class OtFwEventQueue : public OtConcurrentQueue<OtFwEvent> {
public:
	inline void pushMouseButtonEvent(int button, int action, int mods, float x, float y) {
		OtFwEvent event;
		event.type = OtFwEvent::Type::mouseButton;
		event.mouseButton.button = button;
		event.mouseButton.action = action;
		event.mouseButton.mods = mods;
		event.mouseButton.x = x;
		event.mouseButton.y = y;
		push(event);
	}

	inline void pushMouseMoveEvent(float x, float y) {
		OtFwEvent event;
		event.type = OtFwEvent::Type::mouseMove;
		event.mouseMove.x = x;
		event.mouseMove.y = y;
		push(event);
	}

	inline void pushMouseWheelEvent(float xOffset, float yOffset) {
		OtFwEvent event;
		event.type = OtFwEvent::Type::mouseWheel;
		event.mouseWheel.xOffset = xOffset;
		event.mouseWheel.yOffset = yOffset;
		push(event);
	}

	inline void pushKeyboardEvent(int key, int scancode, int action, int mods) {
		OtFwEvent event;
		event.type = OtFwEvent::Type::keyboard;
		event.keyboard.key = key;
		event.keyboard.scancode = scancode;
		event.keyboard.action = action;
		event.keyboard.mods = mods;
		push(event);
	}

	inline void pushCharacterEvent(int codepoint) {
		OtFwEvent event;
		event.type = OtFwEvent::Type::character;
		event.character.codepoint = codepoint;
		push(event);
	}

	inline void pushGamepadAxisEvent(int gamepad, int axis, int value) {
		OtFwEvent event;
		event.type = OtFwEvent::Type::gamepadAxis;
		event.gamepadAxis.gamepad = gamepad;
		event.gamepadAxis.axis = axis;
		event.gamepadAxis.value = value;
		push(event);
	}

	inline void pushGamepadButtonEvent(int gamepad, int button, int action) {
		OtFwEvent event;
		event.type = OtFwEvent::Type::gamepadButton;
		event.gamepadButton.gamepad = gamepad;
		event.gamepadButton.button = button;
		event.gamepadButton.action = action;
		push(event);
	}
};
