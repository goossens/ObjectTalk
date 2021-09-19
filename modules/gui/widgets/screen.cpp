//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
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
	if (!(child->isKindOf("Menubar") || child->isKindOf("Window") || child->isKindOf("Panel") || child->isKindOf("View"))) {
		OtExcept("A [Screen] can only have [Menubar], [Window], [Panel] or [View] instances as children, not a [%s]", child->getType()->getName().c_str());
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

void OtScreenClass::onMouseButton(int button, int action, int mods, double xpos, double ypos) {
	for (auto& child: children) {
		if (child->isKindOf("View")) {
			child->cast<OtViewClass>()->onMouseButton(button, action, mods, xpos, ypos);
		}
	}
}


//
//	OtScreenClass::onMouseMove
//

void OtScreenClass::onMouseMove(double xpos, double ypos) {
	for (auto& child: children) {
		if (child->isKindOf("View")) {
			child->cast<OtViewClass>()->onMouseMove(xpos, ypos);
		}
	}
}


//
//	OtScreenClass::onMouseDrag
//

void OtScreenClass::onMouseDrag(int button, int mods, double xpos, double ypos) {
	for (auto& child: children) {
		if (child->isKindOf("View")) {
			child->cast<OtViewClass>()->onMouseDrag(button, mods, xpos, ypos);
		}
	}
}


//
//	OtScreenClass::onKey
//

void OtScreenClass::onKey(int key, int mods) {
	for (auto& child: children) {
		if (child->isKindOf("View")) {
			child->cast<OtViewClass>()->onKey(key, mods);
		}
	}
}


//
//	OtScreenClass::onChar
//

void OtScreenClass::onChar(unsigned int codepoint) {
	for (auto& child: children) {
		if (child->isKindOf("View")) {
			child->cast<OtViewClass>()->onChar(codepoint);
		}
	}
}


//
//	OtScreenClass::getMeta
//

OtType OtScreenClass::getMeta() {
	static OtType type = nullptr;

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
