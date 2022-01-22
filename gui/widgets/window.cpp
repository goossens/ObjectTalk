//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/function.h"

#include "window.h"


//
//	OtWindowClass::validateChild
//

void OtWindowClass::validateChild(OtComponent child) {
	if (!child->isKindOf("Widget")) {
		OtExcept("A [Window] can only have [Widget] subclasses as children, not [%s]", child->getType()->getName().c_str());
	}
}


//
//	OtWindowClass::render
//

void OtWindowClass::render() {
	// position window
	ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_FirstUseEver);

	if (ImGui::Begin(title.c_str(), nullptr, flags)) {
		// render all children
		OtWidgetClass::render();
	}

	ImGui::End();
}


//
//	OtWindowClass::getMeta
//

OtType OtWindowClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtWindowClass>("Window", OtScreenObjectClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtWindowClass::init));
		type->set("setTitle", OtFunctionClass::create(&OtWindowClass::setTitle));
		type->set("getTitle", OtFunctionClass::create(&OtWindowClass::getTitle));
		type->set("setSize", OtFunctionClass::create(&OtWindowClass::setSize));
		type->set("getWidth", OtFunctionClass::create(&OtWindowClass::getWidth));
		type->set("getHeight", OtFunctionClass::create(&OtWindowClass::getHeight));
		type->set("setFlags", OtFunctionClass::create(&OtWindowClass::setFlags));
		type->set("getFlags", OtFunctionClass::create(&OtWindowClass::getFlags));
	}

	return type;
}


//
//	OtWindowClass::create
//

OtWindow OtWindowClass::create() {
	OtWindow widget = std::make_shared<OtWindowClass>();
	widget->setType(getMeta());
	return widget;
}
