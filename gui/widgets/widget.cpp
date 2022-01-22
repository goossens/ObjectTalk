//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>

#include "ot/function.h"

#include "widget.h"


//
//	OtWidgetClass::validateChild
//

void OtWidgetClass::validateChild(OtComponent child) {
	OtExcept("A [%s] can't have children", child->getType()->getName().c_str());
}


//
//	OtWidgetClass::update
//

void OtWidgetClass::update() {
	// update all children
	for (auto const& child : children) {
		if (child->isEnabled()) {
			child->cast<OtWidgetClass>()->update();
		}
	}
}


//
//	OtWidgetClass::render
//

void OtWidgetClass::render() {
	// render all children
	for (auto const& child : children) {
		if (child->isEnabled()) {
			child->cast<OtWidgetClass>()->render();
		}
	}
}


//
//	OtWidgetClass::getMeta
//

OtType OtWidgetClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtWidgetClass>("Widget", OtComponentClass::getMeta());
	}

	return type;
}


//
//	OtWidgetClass::create
//

OtWidget OtWidgetClass::create() {
	OtWidget widget = std::make_shared<OtWidgetClass>();
	widget->setType(getMeta());
	return widget;
}
