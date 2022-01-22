//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/function.h"
#include "ot/vm.h"

#include "realslider.h"


//
//	OtRealSliderClass::init
//

void OtRealSliderClass::init(const std::string& l, float mn, float mx, float v, OtObject cb) {
	// save label and callback
	label = l;
	min = mn;
	max = mx;
	value = v;
	callback = cb;
}


//
//	OtRealSliderClass::render
//

void OtRealSliderClass::render() {
	if (ImGui::SliderFloat(label.c_str(), &value, min, max)) {
		OtVM::instance()->callMemberFunction(callback, "__call__", OtObjectCreate(value));
	}
}


//
//	OtRealSliderClass::getMeta
//

OtType OtRealSliderClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtRealSliderClass>("RealSlider", OtWidgetClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtRealSliderClass::init));
	}

	return type;
}


//
//	OtRealSliderClass::create
//

OtRealSlider OtRealSliderClass::create() {
	OtRealSlider slider = std::make_shared<OtRealSliderClass>();
	slider->setType(getMeta());
	return slider;
}
