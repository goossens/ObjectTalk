//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtTexture.h"
#include "OtWidget.h"


//
//	OtMeterClass
//

class OtMeterClass;
typedef std::shared_ptr<OtMeterClass> OtMeter;

class OtMeterClass : public OtWidgetClass {
public:
	// initialize
	void init(size_t count, OtObject* parameters);

	// specify a new texture
	OtObject setTexture(const std::string& texture);

	// set the margin around the meter
	OtObject setMargin(int margin);

	// specify a label for the meter
	OtObject setLabel(const std::string& label);

	// access measurement (range 0-100)
	OtObject setValue(float v) { value = v; return shared(); }
	float getValue() { return value; }

	// render content
	void render() override;

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtMeter create();

private:
	// properties
	OtTexture texture;
	int margin = 5;
	float value;
	std::string label;

	size_t width;
	size_t height;
};
