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
//	OtBlitClass
//

class OtBlitClass;
typedef std::shared_ptr<OtBlitClass> OtBlit;

class OtBlitClass : public OtFilterClass {
public:
	// constructor/destructor
	OtBlitClass();
	~OtBlitClass();

	// set properties
	void setIntensity(float i) { intensity = i; }
	void setAlpha(float a) { alpha = a; }

	// execute filter
	void execute(int view, int w, int h);

private:
	// blur properties
	float intensity = -1.0;
	float alpha = -1.0;

	// BGFX shader
	bgfx::UniformHandle blitUniform = BGFX_INVALID_HANDLE;
	bgfx::ProgramHandle shader = BGFX_INVALID_HANDLE;
};
