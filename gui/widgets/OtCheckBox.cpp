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
#include "OtVM.h"

#include "OtUi.h"

#include "OtCheckBox.h"


//
//	OtCheckBoxClass::init
//

void OtCheckBoxClass::init(size_t count, OtObject* parameters) {
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
			OtLogFatal("[CheckBox] constructor expects 3 or less arguments (not {})", count);
	}
}


//
//	OtCheckBoxClass::render
//

void OtCheckBoxClass::render() {
	ImGui::PushID(this);

	if (OtUi::toggleButton(label.c_str(), &checked)) {
		OtVM::callMemberFunction(callback, "__call__", OtValue<bool>::encode(checked));
	}

	ImGui::PopID();
}


//
//	OtCheckBoxClass::getMeta
//

OtType OtCheckBoxClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtCheckBoxClass>("CheckBox", OtWidgetClass::getMeta());
		type->set("__init__", OtFunction::create(&OtCheckBoxClass::init));
		type->set("setLabel", OtFunction::create(&OtCheckBoxClass::setLabel));
		type->set("setChecked", OtFunction::create(&OtCheckBoxClass::setChecked));
		type->set("setCallback", OtFunction::create(&OtCheckBoxClass::setCallback));
		type->set("isChecked", OtFunction::create(&OtCheckBoxClass::isChecked));
	}

	return type;
}
