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
//	OtBlur
//

class OtBlur : public OtFilter {
public:
	// constructor
	OtBlur();

	// set properties
	inline void setIntensity(float i) { intensity = i; }
	inline void setAlpha(float a) { alpha = a; }
	inline void setDirection(const glm::vec2& d) { direction = d; }

private:
	// execute filter
	void execute(OtPass& pass) override;

	// properties
	float intensity = 2.0f;
	float alpha = 1.0f;
	glm::vec2 direction{1.0f};

	// GPU assets
	OtUniformVec4 uniform = OtUniformVec4("u_blur", 1);
	OtShaderProgram program = OtShaderProgram("OtFilterVS", "OtBlurFS");
};
