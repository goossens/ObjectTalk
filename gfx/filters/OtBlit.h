//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
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
//	OtBlit
//

class OtBlit : public OtFilter {
public:
	// set properties
	void setIntensity(float i) { intensity = i; }
	void setAlpha(float a) { alpha = a; }

private:
	// execute filter
	void execute(OtPass& pass, int w, int h) override;

	// blur properties
	float intensity = -1.0;
	float alpha = -1.0;

	// GPU assets
	OtUniformVec4 uniform = OtUniformVec4("u_blit", 1);
	OtShaderProgram program = OtShaderProgram("OtBlitVS", "OtBlitFS");
};
