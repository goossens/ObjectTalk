//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/function.h"

#include "screen.h"
#include "menubar.h"
#include "view.h"


//
//	OtScreenClass::validateChild
//

void OtScreenClass::validateChild(OtComponent child) {
	if (!(child->isKindOf("ScreenObject"))) {
		OtExcept("A [Screen] can only have [ScreenObject] instances as children, not a [%s]", child->getType()->getName().c_str());
	}
}


//
//	OtScreenClass::getMenubarHeight
//

size_t OtScreenClass::getMenubarHeight() {
	for (auto& child: children) {
		if (child->isKindOf("Menubar")) {
			return child->cast<OtMenubarClass>()->getHeight();
		}
	}

	return 0;
}


//
//	OtScreenClass::onMouseButton
//

void OtScreenClass::onMouseButton(int button, int action, int mods, float xpos, float ypos) {
	bool handled = false;

	for (auto it = children.begin(); !handled && it != children.end(); it++) {
		handled = (*it)->cast<OtScreenObjectClass>()->onMouseButton(button, action, mods, xpos, ypos);
	}
}


//
//	OtScreenClass::onMouseMove
//

void OtScreenClass::onMouseMove(float xpos, float ypos) {
	bool handled = false;

	for (auto it = children.begin(); !handled && it != children.end(); it++) {
		handled = (*it)->cast<OtScreenObjectClass>()->onMouseMove(xpos, ypos);
	}
}


//
//	OtScreenClass::onMouseDrag
//

void OtScreenClass::onMouseDrag(int button, int mods, float xpos, float ypos) {
	bool handled = false;

	for (auto it = children.begin(); !handled && it != children.end(); it++) {
		handled = (*it)->cast<OtScreenObjectClass>()->onMouseDrag(button, mods, xpos, ypos);
	}
}


//
//	OtScreenClass::onScrollWheel
//

void OtScreenClass::onScrollWheel(float dx, float dy){
	bool handled = false;

	for (auto it = children.begin(); !handled && it != children.end(); it++) {
		handled = (*it)->cast<OtScreenObjectClass>()->onScrollWheel(dx, dy);
	}
}


//
//	OtScreenClass::onKey
//

void OtScreenClass::onKey(int key, int mods) {
	bool handled = false;

	for (auto it = children.begin(); !handled && it != children.end(); it++) {
		handled = (*it)->cast<OtScreenObjectClass>()->onKey(key, mods);
	}
}


//
//	OtScreenClass::onChar
//

void OtScreenClass::onChar(unsigned int codepoint) {
	bool handled = false;

	for (auto it = children.begin(); !handled && it != children.end(); it++) {
		handled = (*it)->cast<OtScreenObjectClass>()->onChar(codepoint);
	}
}


//
//	OtScreenClass::onGamepadAxis
//

void OtScreenClass::onGamepadAxis(int gamepad, int axis, int value) {
	bool handled = false;

	for (auto it = children.begin(); !handled && it != children.end(); it++) {
		handled = (*it)->cast<OtScreenObjectClass>()->onGamepadAxis(gamepad, axis, value);
	}
}


//
//	OtScreenClass::onGamepadButton
//

void OtScreenClass::onGamepadButton(int gamepad, int button, int action) {
	bool handled = false;

	for (auto it = children.begin(); !handled && it != children.end(); it++) {
		handled = (*it)->cast<OtScreenObjectClass>()->onGamepadButton(gamepad, button, action);
	}
}


//
//	OtScreenClass::getMeta
//

OtType OtScreenClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtScreenClass>("Screen", OtWidgetClass::getMeta());
	}

	return type;
}


//
//	OtScreenClass::create
//

OtScreen OtScreenClass::create() {
	OtScreen screen = std::make_shared<OtScreenClass>();
	screen->setType(getMeta());
	return screen;
}
