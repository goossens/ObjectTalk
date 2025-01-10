//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"

#include "OtFunction.h"

#include "OtUi.h"

#include "OtProperties.h"


//
//	OtPropertiesClass::init
//

void OtPropertiesClass::init(size_t count, OtObject* parameters) {
	switch (count) {
		case 1:
			setSplit(parameters[0]->operator float());

		case 0:
			break;

		default:
			OtLogFatal("[ComboBox] constructor expects up to 1 argument (not {})", count);
	}
}


//
//	OtPropertiesClass::render
//

void OtPropertiesClass::render() {
	// render all children
	ImGui::PushID(this);
	ImGui::BeginChild("panel");
	ImGui::PushItemWidth(ImGui::GetWindowWidth() * split);

	renderChildren();

	ImGui::PopItemWidth();
	ImGui::EndChild();
	ImGui::PopID();
}


//
//	OtPropertiesClass::getMeta
//

OtType OtPropertiesClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtPropertiesClass>("Properties", OtWidgetClass::getMeta());
		type->set("__init__", OtFunction::create(&OtPropertiesClass::init));
		type->set("setSplit", OtFunction::create(&OtPropertiesClass::setSplit));
	}

	return type;
}
