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

#include "OtRealSlider.h"


//
//	OtRealSliderClass::init
//

void OtRealSliderClass::init(const std::string& l, float mn, float mx, float v, OtObject cb) {
	// save label and callback
	label = l;
	min = mn;
	max = mx;
	value = v;
	OtCallbackValidate(cb, 1);
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
		type->set("__init__", OtFunction::create(&OtRealSliderClass::init));
	}

	return type;
}
