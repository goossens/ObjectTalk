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

#include "OtHeader.h"


//
//	OtHeaderClass::init
//

void OtHeaderClass::init(size_t count, OtObject* parameters) {
	switch (count) {
		case 1:
			setLabel(parameters[0]->operator std::string());
			[[fallthrough]];

		case 0:
			break;

		default:
		OtLogError("[Header] constructor expects up to 1 argument (not {})", count);
	}
}


//
//	OtHeaderClass::render
//

void OtHeaderClass::render() {
	ImGui::PushID(this);
	OtUi::header(label.c_str());
	ImGui::Dummy(ImVec2(0.0f, 0.0f));
	ImGui::PopID();
}


//
//	OtHeaderClass::getMeta
//

OtType OtHeaderClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtHeaderClass>("Header", OtWidgetClass::getMeta());
		type->set("__init__", OtFunction::create(&OtHeaderClass::init));
		type->set("setLabel", OtFunction::create(&OtHeaderClass::setLabel));
	}

	return type;
}
