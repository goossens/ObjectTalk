//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtWidget.h"


//
//	OtRealSliderClass
//

class OtRealSliderClass;
typedef std::shared_ptr<OtRealSliderClass> OtRealSlider;

class OtRealSliderClass : public OtWidgetClass {
public:
	// initialize slider
	void init(const std::string& label, float min, float max, float value, OtObject callback);

	// render content
	void render() override;

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtRealSlider create();

private:
	// properties
	std::string label;
	float min;
	float max;
	float value;
	OtObject callback;
};
