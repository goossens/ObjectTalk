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
#include "OtRgbaAdjustComp.h"


//
//	OtRgbaAdjust
//

class OtRgbaAdjust : public OtFilter {
public:
	// set properties
	inline void setRgba(glm::vec4 value) { rgba = value; }

	// configure the compute pass
	void configurePass(OtComputePass& pass) override {
		// initialize pipeline (if required)
		if (!pipeline.isValid()) {
			pipeline.setShader(OtRgbaAdjustComp, sizeof(OtRgbaAdjustComp));
		}

		// set uniforms
		struct Uniforms {
			glm::vec4 rgba;
		} uniforms {
			rgba
		};

		pass.addUniforms(&uniforms, sizeof(uniforms));
	}

private:
	// properties
	glm::vec4 rgba{1.0f};
};
