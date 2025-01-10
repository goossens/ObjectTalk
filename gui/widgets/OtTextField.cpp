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

#include "OtTextField.h"


//
//	OtTextFieldClass::init
//

void OtTextFieldClass::init(size_t count, OtObject* parameters) {
	switch (count) {
		case 3:
			setCallback(parameters[2]);

		case 2:
			setValue(parameters[1]-> operator std::string());

		case 1:
			setLabel(parameters[0]->operator std::string());

		case 0:
			break;

		default:
			OtLogFatal("[TextField] constructor expects up to 3 arguments (not {})", count);
	}
}


//
//	OtTextFieldClass::render
//

void OtTextFieldClass::render() {
	ImGui::PushID(this);

	if (OtUi::inputText(label.c_str(), &value)) {
		OtVM::callMemberFunction(callback, "__call__", OtValue<std::string>::encode(value));
	}

	ImGui::PopID();
}


//
//	OtTextFieldClass::getMeta
//

OtType OtTextFieldClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtTextFieldClass>("TextField", OtWidgetClass::getMeta());
		type->set("__init__", OtFunction::create(&OtTextFieldClass::init));
		type->set("setLabel", OtFunction::create(&OtTextFieldClass::setLabel));
		type->set("setValue", OtFunction::create(&OtTextFieldClass::setValue));
		type->set("setCallback", OtFunction::create(&OtTextFieldClass::setCallback));
		type->set("getValue", OtFunction::create(&OtTextFieldClass::getValue));
	}

	return type;
}
