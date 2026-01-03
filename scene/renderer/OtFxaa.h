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

#include "OtFilter.h"
#include "OtShaders.h"


//
//	OtFxaa
//

class OtFxaa : public OtFilter {
public:
	// configure the compute pass
	void configurePass(OtComputePass& pass) override {
		// initialize pipeline (if required)
		if (!pipeline.isValid()) {
			pipeline.setShader(OtFxaaComp, OtFxaaCompSize);
		}

		// set uniforms
		struct Uniforms {
			glm::vec2 TexelSize;
		} uniforms {
			sourceTexelSize
		};

		pass.addUniforms(&uniforms, sizeof(uniforms));
	}
};
