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
//	OtGaussian
//

class OtGaussian : public OtFilter {
public:
	// constructor
	OtGaussian();

	// set properties
	inline void setRadius(float r) { radius = r; }
	inline void setDirection(const glm::vec2& d) { direction = d; }

private:
	// execute filter
	void execute(OtPass& pass) override;

	// properties
	float radius = 1.0f;
	glm::vec2 direction{1.0f};

	// GPU assets
	OtUniformVec4 uniform = OtUniformVec4("u_gaussian", 1);
	OtShaderProgram program = OtShaderProgram("OtFilterVS", "OtGaussian9FS");
};
