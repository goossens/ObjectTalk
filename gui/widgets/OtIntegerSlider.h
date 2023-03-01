//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtWidget.h"


//
//	OtIntegerSliderClass
//

class OtIntegerSliderClass;
using OtIntegerSlider = OtObjectPointer<OtIntegerSliderClass>;

class OtIntegerSliderClass : public OtWidgetClass {
public:
	// initialize slider
	void init(const std::string& label, int min, int max, int value, OtObject callback);

	// render content
	void render() override;

	// get type definition
	static OtType getMeta();

private:
	// properties
	std::string label;
	int min;
	int max;
	int value;
	OtObject callback;
};
