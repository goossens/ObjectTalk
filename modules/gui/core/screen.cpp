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
//	OtScreenClass::getMeta
//

OtType OtScreenClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtScreenClass>("Screen", OtCompositeClass::getMeta());
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
