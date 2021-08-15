//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/function.h"

#include "menubar.h"


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
	// see what type of menubar we need
	if (parent->isKindOf("Screen")) {
		// it's a screen-level main menubar
		if (ImGui::BeginMainMenuBar()) {
			height = ImGui::GetWindowSize().y;
			OtWidgetClass::render();
			ImGui::EndMainMenuBar();
		}

	} else {
		// it's a regular menubar
		if (ImGui::BeginMenuBar()) {
			height = ImGui::GetWindowSize().y;
			OtWidgetClass::render();
			ImGui::EndMenuBar();
		}
	}
}


//
//	OtMenubarClass::getMeta
//

OtType OtMenubarClass::getMeta() {
	static OtType type = nullptr;

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
