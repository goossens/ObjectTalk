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
#include "OtShaders.h"


//
//	OtNeedle
//

class OtNeedle : public OtFilter {
public:
	// set properties
	inline void setNeedle(OtTexture value) { needleTexture = value; }
	inline void setTransform(const glm::mat4& value) { transform = value; }

	// configure the compute pass
	void configurePass(OtComputePass& pass) override {
		// initialize pipeline (if required)
		if (!pipeline.isValid()) {
			pipeline.setShader(OtNeedleComp, OtNeedleCompSize);
		}

		// add overlay texture
		pass.addInputSampler(needleSampler, needleTexture);

		// set uniforms
		struct Uniforms {
			glm::mat4 transform;
		} uniforms {
			transform
		};

		pass.addUniforms(&uniforms, sizeof(uniforms));
	}

private:
	// properties
	glm::mat4 transform{1.0f};
	OtTexture needleTexture;
	OtSampler needleSampler{OtSampler::Filter::nearest, OtSampler::Addressing::clamp};
};
