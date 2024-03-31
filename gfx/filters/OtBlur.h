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
//	OtBlur
//

class OtBlur : public OtFilter {
public:
	// set properties
	inline void setHorizontalScale(float scale) { horizontalScale = scale; }
	inline void setVerticalScale(float scale) { verticalScale = scale; }
	inline void setIntensity(float i) { intensity = i; }
	inline void setAlpha(float a) { alpha = a; }

private:
	// execute filter
	void execute(OtPass& pass) override;

	// blur properties
	float horizontalScale = -1.0f;
	float verticalScale = -1.0f;
	float intensity = -1.0f;
	float alpha = -1.0f;

	// GPU assets
	OtUniformVec4 uniform = OtUniformVec4("u_blur", 1);
	OtShaderProgram program = OtShaderProgram("OtFilterVS", "OtBlurFS");
};
