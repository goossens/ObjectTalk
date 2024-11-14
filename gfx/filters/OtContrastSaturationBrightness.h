//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtFilter.h"
#include "OtShaderProgram.h"
#include "OtUniformVec4.h"


//
//	OtContrastSaturationBrightness
//

class OtContrastSaturationBrightness : public OtFilter {
public:
	// set properties
	inline void setContrast(float c) { contrast = c; }
	inline void setSaturation(float s) { saturation = s; }
	inline void setBrightness(float b) { brightness = b; }

private:
	// execute filter
	void execute(OtPass& pass) override;

	// filter properties
	float contrast = 1.0f;
	float saturation = 1.0f;
	float brightness = 1.0f;

	// GPU assets
	OtUniformVec4 uniform = OtUniformVec4("u_csb", 1);
	OtShaderProgram program = OtShaderProgram("OtFilterVS", "OtContrastSaturationBrightnessFS");
};
