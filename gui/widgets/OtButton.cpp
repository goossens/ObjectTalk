//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"

#include "OtFunction.h"
#include "OtVM.h"

#include "OtUi.h"

#include "OtButton.h"


//
//	OtButtonClass::init
//

void OtButtonClass::init(size_t count, OtObject* parameters) {
	switch (count) {
		case 2:
			setCallback(parameters[1]);
			[[fallthrough]];

		case 1:
			setLabel(parameters[0]->operator std::string());
			[[fallthrough]];

		case 0:
			break;

		default:
			OtLogError("[Button] constructor expects 2 or less arguments (not {})", count);
	}
}


//
//	OtButtonClass::render
//

void OtButtonClass::render() {
	ImGui::PushID(this);

	if (ImGui::Button(label.c_str())) {
		OtVM::callMemberFunction(callback, "__call__");
	}

	ImGui::PopID();
}


//
//	OtButtonClass::getMeta
//

OtType OtButtonClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtButtonClass>("Button", OtWidgetClass::getMeta());
		type->set("__init__", OtFunction::create(&OtButtonClass::init));
		type->set("setLabel", OtFunction::create(&OtButtonClass::setLabel));
		type->set("setCallback", OtFunction::create(&OtButtonClass::setCallback));
	}

	return type;
}
