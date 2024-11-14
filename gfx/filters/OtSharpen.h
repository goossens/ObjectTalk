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
//	OtSharpen
//

class OtSharpen : public OtFilter {
public:
	// set properties
	inline void setStrength(float s) { strength = s; }

private:
	// execute filter
	void execute(OtPass& pass) override;

	// filter properties
	float strength = 1.0f;

	// GPU assets
	OtUniformVec4 uniform = OtUniformVec4("u_sharpen", 1);
	OtShaderProgram program = OtShaderProgram("OtFilterVS", "OtSharpenFS");
};
