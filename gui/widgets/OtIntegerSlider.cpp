//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"

#include "OtCallback.h"
#include "OtFunction.h"
#include "OtVM.h"

#include "OtIntegerSlider.h"


//
//	OtIntegerSliderClass::init
//

void OtIntegerSliderClass::init(const std::string& l, int mn, int mx, int v, OtObject cb) {
	// save label and callback
	label = l;
	min = mn;
	max = mx;
	value = v;

	OtCallbackValidate(cb, 1);
	callback = cb;
}


//
//	OtIntegerSliderClass::render
//

void OtIntegerSliderClass::render() {
	if (ImGui::SliderInt(label.c_str(), &value, min, max)) {
		OtVM::instance()->callMemberFunction(callback, "__call__", OtObjectCreate(value));
	}
}


//
//	OtIntegerSliderClass::getMeta
//

OtType OtIntegerSliderClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtIntegerSliderClass>("IntegerSlider", OtWidgetClass::getMeta());
		type->set("__init__", OtFunction::create(&OtIntegerSliderClass::init));
	}

	return type;
}
