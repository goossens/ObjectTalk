//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtException.h"

#include "OtAppObject.h"
#include "OtWidget.h"


//
//	OtAppObjectClass::validateChild
//

void OtAppObjectClass::validateChild(OtComponent child) {
	OtExcept("A [%s] can't have children", getType()->getName().c_str());
}


//
//	OtAppObjectClass::update
//

void OtAppObjectClass::update() {
	// update all children
	for (auto const& child : children) {
		if (child->isEnabled()) {
			child->cast<OtWidgetClass>()->update();
		}
	}
}


//
//	OtAppObjectClass::render
//

void OtAppObjectClass::render() {
	// render all children
	for (auto const& child : children) {
		if (child->isEnabled()) {
			child->cast<OtWidgetClass>()->render();
		}
	}
}


//
//	OtAppObjectClass::getMeta
//

OtType OtAppObjectClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtAppObjectClass>("AppObject", OtComponentClass::getMeta());
	}

	return type;
}
