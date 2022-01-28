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

#include "OtFilter.h"


//
//	OtBlurClass
//

class OtBlurClass;
typedef std::shared_ptr<OtBlurClass> OtBlur;

class OtBlurClass : public OtFilterClass {
public:
	// constructor/destructor
	OtBlurClass();
	~OtBlurClass();

	// set properties
	void setHorizontalScale(float scale) { horizontalScale = scale; }
	void setVerticalScale(float scale) { verticalScale = scale; }
	void setIntensity(float i) { intensity = i; }
	void setAlpha(float a) { alpha = a; }

	// execute filter
	void execute(int view, int w, int h);

private:
	// blur properties
	float horizontalScale = -1.0;
	float verticalScale = -1.0;
	float intensity = -1.0;
	float alpha = -1.0;

	// BGFX shader
	bgfx::UniformHandle blurUniform = BGFX_INVALID_HANDLE;
	bgfx::ProgramHandle shader = BGFX_INVALID_HANDLE;
};
