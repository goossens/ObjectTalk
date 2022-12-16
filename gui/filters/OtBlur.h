//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtFilter.h"
#include "OtShader.h"
#include "OtUniform.h"


//
//	OtBlurClass
//

class OtBlurClass;
typedef std::shared_ptr<OtBlurClass> OtBlur;

class OtBlurClass : public OtFilterClass {
public:
	// set properties
	void setHorizontalScale(float scale) { horizontalScale = scale; }
	void setVerticalScale(float scale) { verticalScale = scale; }
	void setIntensity(float i) { intensity = i; }
	void setAlpha(float a) { alpha = a; }

	// execute filter
	void execute(OtPass& pass, int w, int h) override;

private:
	// blur properties
	float horizontalScale = -1.0;
	float verticalScale = -1.0;
	float intensity = -1.0;
	float alpha = -1.0;

	// GPU assets
	OtUniform uniform = OtUniform("u_blur", 1);
	OtShader shader = OtShader("OtBlurVS", "OtBlurFS");
};
