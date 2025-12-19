//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtFilter.h"
#include "OtSampler.h"
#include "OtTexture.h"

#include "OtShaders.h"


//
//	OtGodRays
//

class OtGodRays : public OtFilter {
public:
	// set properties
	inline void setLightColor(glm::vec4 value) { lightColor = value; }
	inline void setLightUv(glm::vec2 value) { lightUv = value; }
	inline void setDecay(float value) { decay = value; }
	inline void setExposure(float value) { exposure = value; }
	inline void setDensity(float value) { density = value; }
	inline void setWeight(float value) { weight = value; }

	inline void setOcclusion(OtTexture value) { occlusion = value; }

	// configure the compute pass
	void configurePass(OtComputePass& pass) override {
		// initialize pipeline (if required)
		if (!pipeline.isValid()) {
			pipeline.setShader(OtGodRaysComp, OtGodRaysCompSize);
		}

		// set uniforms
		struct Uniforms {
			glm::vec4 lightColor;
			glm::vec2 lightUv;
			float decay;
			float exposure;
			float density;
			float weight;

		} uniforms {
			lightColor,
			lightUv,
			decay,
			exposure,
			density,
			weight
		};

		pass.addUniforms(&uniforms, sizeof(uniforms));
		pass.addInputSampler(sampler, occlusion);
	}

private:
	// properties
	glm::vec4 lightColor{1.0f};
	glm::vec2 lightUv{0.5f};
	float decay = 0.92f;
	float exposure =  0.5f;
	float density = 0.9f;
	float weight=  0.5f;

	OtTexture occlusion;
	OtSampler sampler{OtSampler::Filter::nearest, OtSampler::Addressing::clamp};
};
