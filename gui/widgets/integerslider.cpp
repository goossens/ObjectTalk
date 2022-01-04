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

#include "integerslider.h"


//
//	OtIntegerSliderClass::init
//

void OtIntegerSliderClass::init(const std::string& l, int mn, int mx, int v, OtObject cb) {
	// save label and callback
	label = l;
	min = mn;
	max = mx;
	value = v;
	callback = cb;
}


//
//	OtIntegerSliderClass::render
//

void OtIntegerSliderClass::render() {
	if (ImGui::SliderInt(label.c_str(), &value, min, max)) {
		OtVM::callMemberFunction(callback, "__call__", OtObjectCreate(value));
	}
}


//
//	OtIntegerSliderClass::getMeta
//

OtType OtIntegerSliderClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtIntegerSliderClass>("IntegerSlider", OtWidgetClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtIntegerSliderClass::init));
	}

	return type;
}


//
//	OtIntegerSliderClass::create
//

OtIntegerSlider OtIntegerSliderClass::create() {
	OtIntegerSlider slider = std::make_shared<OtIntegerSliderClass>();
	slider->setType(getMeta());
	return slider;
}
