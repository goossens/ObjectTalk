//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"

#include "OtFunction.h"

#include "OtMenubar.h"


//
//	OtMenubarClass::validateChild
//

void OtMenubarClass::validateChild(OtComponent child) {
	if (!child->isKindOf("Menu")) {
		OtExcept("A [Menubar] can only have [Menu]s as children, not [%s]", child->getType()->getName().c_str());
	}
}


//
//	OtMenubarClass::render
//

void OtMenubarClass::render() {
	if (ImGui::BeginMenuBar()) {
		height = ImGui::GetWindowSize().y;
		OtWidgetClass::render();
		ImGui::EndMenuBar();
	}
}


//
//	OtMenubarClass::getMeta
//

OtType OtMenubarClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtMenubarClass>("Menubar", OtWidgetClass::getMeta());
	}

	return type;
}


//
//	OtMenubarClass::create
//

OtMenubar OtMenubarClass::create() {
	OtMenubar menubar = std::make_shared<OtMenubarClass>();
	menubar->setType(getMeta());
	return menubar;
}
