//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>

#include "OtCallback.h"

#include "OtWidget.h"


//
//	OtRealSliderClass
//

class OtRealSliderClass;
using OtRealSlider = OtObjectPointer<OtRealSliderClass>;

class OtRealSliderClass : public OtWidgetClass {
public:
	// initialize
	void init(size_t count, OtObject* parameters);

	// access properties
	inline OtObject setLabel(const std::string& l) {
		label = l;
		return OtWidget(this);
	}

	inline OtObject setValue(float v) {
		value = v;
		return OtWidget(this);
	}

	inline OtObject setValueLimits(float mn, float mx) {
		minValue = mn;
		maxValue = mx;
		return OtWidget(this);
	}

	inline OtObject setCallback(OtObject cb) {
		OtCallbackValidate(cb, 1);
		callback = cb;
		return OtWidget(this);
	}

	inline int getValue() { return value; }

	// render content
	void render() override;

	// get type definition
	static OtType getMeta();

private:
	std::string label;
	float value = 0.0f;
	float minValue = 0.0f;
	float maxValue = 10.0f;
	OtObject callback;
};
