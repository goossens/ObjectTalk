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
//	OtLinearizeDepth
//

class OtLinearizeDepth : public OtFilter {
public:
	// set properties
	inline void setNearFar(float n, float f) { nearPlane = n; farPlane = f; }

private:
	// execute filter
	void execute(OtPass& pass) override;

	// blur properties
	float nearPlane = 0.01f;
	float farPlane = 10.0f;

	// GPU assets
	OtUniformVec4 uniform = OtUniformVec4("u_linearize", 1);
	OtShaderProgram program = OtShaderProgram("OtFilterVS", "OtLinearizeDepthFS");
};
