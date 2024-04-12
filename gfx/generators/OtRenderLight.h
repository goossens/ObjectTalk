//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "glm/glm.hpp"

#include "OtGenerator.h"
#include "OtShaderProgram.h"
#include "OtUniformVec4.h"


//
//	OtRenderLight
//

class OtRenderLight : public OtGenerator {
public:
	// set the properties
	inline void setCenter(const glm::vec2& c) { center = c; }
	inline void setSize(const glm::vec2& s) { size = s; }
	inline void setColor(const glm::vec3& c) { color = c; }

private:
	// execute generator
	void execute(OtPass& pass) override;

	// properties
	glm::vec2 center{0.0f};
	glm::vec2 size{0.1f};
	glm::vec3 color{1.0f};

	// shader resources
	OtShaderProgram program{"OtGeneratorVS", "OtRenderLightFS"};
	OtUniformVec4 uniform{"u_renderlight", 2};
};
