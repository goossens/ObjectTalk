//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "glm/glm.hpp"

#include "OtGenerator.h"
#include "OtShaders.h"


//
//	OtRenderLight
//

class OtRenderLight : public OtGenerator {
public:
	// set the properties
	inline void setColor(const glm::vec3& c) { color = c; }
	inline void setCenter(const glm::vec2& c) { center = c; }
	inline void setSize(const glm::vec2& s) { size = s; }

	// configure the compute pass
	void configurePass(OtComputePass& pass) override {
		// initialize pipeline (if required)
		if (!pipeline.isValid()) {
			pipeline.setShader(OtRenderLightComp, OtRenderLightCompSize);
		}

		// set uniforms
		struct Uniforms {
			glm::vec4 color;
			glm::vec2 center;
			glm::vec2 size;
		} uniforms{
			glm::vec4(color, 1.0f),
			center,
			size
		};

		pass.addUniforms(&uniforms, sizeof(uniforms));
	}

private:
	// properties
	glm::vec2 center{0.0f};
	glm::vec2 size{0.1f};
	glm::vec3 color{1.0f};
};
