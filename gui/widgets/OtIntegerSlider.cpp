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
#include "OtVM.h"

#include "OtUi.h"

#include "OtIntegerSlider.h"


//
//	OtIntegerSliderClass::init
//

void OtIntegerSliderClass::init(size_t count, OtObject* parameters) {
	switch (count) {
		case 5:
			setCallback(parameters[4]);

		case 4:
			setValueLimits(
				parameters[2]-> operator int(),
				parameters[3]-> operator int());

		case 2:
			setValue(parameters[1]-> operator int());

		case 1:
			setLabel(parameters[0]->operator std::string());

		case 0:
			break;

		default:
			OtLogFatal("[IntegerSlider] constructor expects 0, 1, 2, 4 or 5 arguments (not {})", count);
	}
}


//
//	OtIntegerSliderClass::render
//

void OtIntegerSliderClass::render() {
	ImGui::PushID(this);

	if (OtUiDragInt(label.c_str(), &value, minValue, maxValue)) {
		OtVM::instance()->callMemberFunction(callback, "__call__", OtValue<int>::encode(value));
	}

	ImGui::PopID();
}


//
//	OtIntegerSliderClass::getMeta
//

OtType OtIntegerSliderClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtIntegerSliderClass>("IntegerSlider", OtWidgetClass::getMeta());
		type->set("__init__", OtFunction::create(&OtIntegerSliderClass::init));
		type->set("setLabel", OtFunction::create(&OtIntegerSliderClass::setLabel));
		type->set("setValue", OtFunction::create(&OtIntegerSliderClass::setValue));
		type->set("setValueLimits", OtFunction::create(&OtIntegerSliderClass::setValueLimits));
		type->set("setCallback", OtFunction::create(&OtIntegerSliderClass::setCallback));
		type->set("getValue", OtFunction::create(&OtIntegerSliderClass::getValue));
	}

	return type;
}
