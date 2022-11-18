//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "bgfx/bgfx.h"

#include "OtWidget.h"


//
//	OtKnobClass
//

class OtKnobClass;
typedef std::shared_ptr<OtKnobClass> OtKnob;

class OtKnobClass : public OtWidgetClass {
public:
	// initialize
	OtObject init(size_t count, OtObject* parameters);

	// specify a new texture
	OtObject setTexture(const std::string& texture);

	// set the margin around the knob
	OtObject setMargin(int margin);

	// specify a label for the knob
	OtObject setLabel(const std::string& label);

	// set callback (called when knob value is changed)
	OtObject setCallback(OtObject callback);

	// access knob value (range 0-100)
	OtObject setValue(float v) { value = v; return shared(); }
	float getValue() { return value; }

	// render content
	void render() override;

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtKnob create();

private:
	// properties
	bgfx::TextureHandle texture = BGFX_INVALID_HANDLE;
	std::string label;
	int margin = 5;
	float value;
	OtObject callback;

	size_t width;
	size_t height;
};
