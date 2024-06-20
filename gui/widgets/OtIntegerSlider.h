//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
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
//	OtIntegerSliderClass
//

class OtIntegerSliderClass;
using OtIntegerSlider = OtObjectPointer<OtIntegerSliderClass>;

class OtIntegerSliderClass : public OtWidgetClass {
public:
	// initialize
	void init(size_t count, OtObject* parameters);

	// access properties
	inline OtObject setLabel(const std::string& l) {
		label = l;
		return OtWidget(this);
	}

	inline OtObject setValue(int v) {
		value = v;
		return OtWidget(this);
	}

	inline OtObject setValueLimits(int mn, int mx) {
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
	int value = 0;
	int minValue = 0;
	int maxValue = 10;
	OtObject callback;
};
