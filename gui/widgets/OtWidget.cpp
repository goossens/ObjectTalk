//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>

#include "OtFunction.h"

#include "OtWidget.h"


//
//	OtWidgetClass::validateChild
//

void OtWidgetClass::validateChild(OtNode child) {
	OtExcept("A [%s] can't have children", getType()->getName().c_str());
}


//
//	OtWidgetClass::update
//

void OtWidgetClass::update() {
	// update all children
	for (auto& child : children) {
		if (child->isEnabled()) {
			OtWidget(child)->update();
		}
	}
}


//
//	OtWidgetClass::render
//

void OtWidgetClass::render() {
	// render all children
	for (auto& child : children) {
		if (child->isEnabled()) {
			OtWidget(child)->render();
		}
	}
}


//
//	OtWidgetClass::getMeta
//

OtType OtWidgetClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtWidgetClass>("Widget", OtNodeClass::getMeta());
	}

	return type;
}
