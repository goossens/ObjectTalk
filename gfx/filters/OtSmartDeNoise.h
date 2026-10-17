//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "glm/glm.hpp" // IWYU pragma: keep

#include "OtFilter.h"
#include "OtShaders.h"


//
//	OtSmartDeNoise
//

class OtSmartDeNoise : public OtFilter {
public:
	// set properties
	inline void setSigma(float value) { sigma = value; }
	inline void setkSigma(float value) { kSigma = value; }
	inline void setThreshold(float value) { threshold = value; }

	// configure the compute pass
	void configurePass(OtComputePass& pass) override {
		// initialize pipeline (if required)
		if (!pipeline.isValid()) {
			pipeline.setShader(OtSmartDeNoiseComp, OtSmartDeNoiseCompSize);
		}

		// set uniforms
		struct Uniforms {
			glm::vec2 texelSize;
			float sigma;
			float kSigma;
			float threshold;
		} uniforms {
			sourceTexelSize,
			sigma,
			kSigma,
			threshold
		};

		pass.addUniforms(&uniforms, sizeof(uniforms));
	}

private:
	// properties
	float sigma = 7.000f;
	float kSigma = 3.0f;
	float threshold = 0.195f;
};
