//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"

#include "OtFunction.h"
#include "OtUi.h"
#include "OtVM.h"

#include "OtCheckbox.h"


//
//	OtCheckboxClass::init
//

void OtCheckboxClass::init(size_t count, OtObject* parameters) {
	switch (count) {
		case 3:
			setCallback(parameters[2]);

		case 2:
			setChecked(parameters[1]-> operator bool());

		case 1:
			setLabel(parameters[0]->operator std::string());

		case 0:
			break;

		default:
			OtLogFatal("[Checkbox] constructor expects 3 or less arguments (not {})", count);
	}
}


//
//	OtCheckboxClass::render
//

void OtCheckboxClass::render() {
	ImGui::PushID(this);

	if (OtUiToggleButton(label.c_str(), &checked)) {
		OtVM::instance()->callMemberFunction(callback, "__call__", OtValue<bool>::encode(checked));
	}

	ImGui::PopID();
}


//
//	OtCheckboxClass::getMeta
//

OtType OtCheckboxClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtCheckboxClass>("Checkbox", OtWidgetClass::getMeta());
		type->set("__init__", OtFunction::create(&OtCheckboxClass::init));
		type->set("setLabel", OtFunction::create(&OtCheckboxClass::setLabel));
		type->set("setChecked", OtFunction::create(&OtCheckboxClass::setChecked));
		type->set("setCallback", OtFunction::create(&OtCheckboxClass::setCallback));
		type->set("isChecked", OtFunction::create(&OtCheckboxClass::isChecked));
	}

	return type;
}
