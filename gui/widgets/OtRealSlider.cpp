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

#include "OtRealSlider.h"


//
//	OtRealSliderClass::init
//

void OtRealSliderClass::init(size_t count, OtObject* parameters) {
	switch (count) {
		case 5:
			setCallback(parameters[4]);

		case 4:
			setValueLimits(
				parameters[2]-> operator float(),
				parameters[3]-> operator float());

		case 2:
			setValue(parameters[1]-> operator float());

		case 1:
			setLabel(parameters[0]->operator std::string());

		case 0:
			break;

		default:
			OtLogFatal("[RealSlider] constructor expects 0, 1, 2, 4 or 5 arguments (not {})", count);
	}
}


//
//	OtRealSliderClass::render
//

void OtRealSliderClass::render() {
	ImGui::PushID(this);

	if (OtUiDragFloat(label.c_str(), &value, minValue, maxValue)) {
		OtVM::instance()->callMemberFunction(callback, "__call__", OtValue<float>::encode(value));
	}

	ImGui::PopID();
}


//
//	OtRealSliderClass::getMeta
//

OtType OtRealSliderClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtRealSliderClass>("RealSlider", OtWidgetClass::getMeta());
		type->set("__init__", OtFunction::create(&OtRealSliderClass::init));
		type->set("setLabel", OtFunction::create(&OtRealSliderClass::setLabel));
		type->set("setValue", OtFunction::create(&OtRealSliderClass::setValue));
		type->set("setValueLimits", OtFunction::create(&OtRealSliderClass::setValueLimits));
		type->set("setCallback", OtFunction::create(&OtRealSliderClass::setCallback));
		type->set("getValue", OtFunction::create(&OtRealSliderClass::getValue));
	}

	return type;
}
