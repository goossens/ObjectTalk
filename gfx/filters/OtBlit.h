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
//	OtBlit
//

class OtBlit : public OtFilter {
public:
	// set properties
	inline void setIntensity(float i) { intensity = i; }
	inline void setAlpha(float a) { alpha = a; }

private:
	// execute filter
	void execute(OtPass& pass) override;

	// blit properties
	float intensity = -1.0;
	float alpha = -1.0;

	// GPU assets
	OtUniformVec4 uniform = OtUniformVec4("u_blit", 1);
	OtShaderProgram program = OtShaderProgram("OtFilterVS", "OtBlitFS");
};
