//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "glm/glm.hpp"

#include "OtFilter.h"
#include "OtShaderProgram.h"
#include "OtUniformVec4.h"


//
//	OtRgba
//

class OtRgba : public OtFilter {
public:
	// set properties
	inline void setRed(float value) { filter.r = value; }
	inline void setGreen(float value) { filter.g = value; }
	inline void setBlue(float value) { filter.b = value; }
	inline void setAlpha(float value) { filter.a = value; }

private:
	// execute filter
	void execute(OtPass& pass) override;

	// properties
	glm::vec4 filter{1.0f};

	// GPU assets
	OtUniformVec4 uniform = OtUniformVec4("u_rgba", 1);
	OtShaderProgram program = OtShaderProgram("OtFilterVS", "OtRgbaFS");
};
