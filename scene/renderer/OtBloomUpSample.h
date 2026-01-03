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
//	OtBloomUpSample
//

class OtBloomUpSample : public OtFilter {
public:
	// set properties
	inline void setIntensity(float value) { intensity = value; }

	// configure the compute pass
	void configurePass(OtComputePass& pass) override {
		// initialize pipeline (if required)
		if (!pipeline.isValid()) {
			pipeline.setShader(OtBloomUpSampleComp, OtBloomUpSampleCompSize);
		}

		// set uniforms
		struct Uniforms {
			glm::vec2 texelSize;
			float intensity;
		} uniforms {
			sourceTexelSize,
			intensity
		};

		pass.addUniforms(&uniforms, sizeof(uniforms));
	}

private:
	float intensity = 1.0f;
};
