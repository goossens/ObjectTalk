//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"

#include "OtFunction.h"

#include "OtMenu.h"


//
//	OtMenuClass::validateChild
//

void OtMenuClass::validateChild(OtComponent child) {
	if (!child->isKindOf("MenuItem")) {
		OtExcept("A [Menu] can only have [MenuItem]s as children, not [%s]", child->getType()->getName().c_str());
	}
}


//
//	OtMenuClass::render
//

void OtMenuClass::render() {
	if (ImGui::BeginMenu(title.c_str(), enabled)) {
		// render all children
		OtWidgetClass::render();

		ImGui::EndMenu();
	}
}


//
//	OtMenuClass::getMeta
//

OtType OtMenuClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtMenuClass>("Menu", OtWidgetClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtMenuClass::init));
		type->set("setTitle", OtFunctionClass::create(&OtMenuClass::setTitle));
		type->set("getTitle", OtFunctionClass::create(&OtMenuClass::getTitle));
	}

	return type;
}


//
//	OtMenuClass::create
//

OtMenu OtMenuClass::create() {
	OtMenu menu = std::make_shared<OtMenuClass>();
	menu->setType(getMeta());
	return menu;
}
