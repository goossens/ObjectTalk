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

#include "OtFogComp.h"
#include "OtFilter.h"


//
//	OtFog
//

class OtFog : public OtFilter {
public:
	// set properties
	inline void setDepthTexture(OtTexture value) { depthTexture = value; }
	inline void setInverseProjection(const glm::mat4& value) { invProj = value; }
	inline void setFogColor(const glm::vec3& value) { fogColor = value; }
	inline void setFogDensity(float value) { fogDensity = value; }
	inline void setBackgroundFogRatio(float value) { backgroundFogRatio = value; }

	// configure the compute pass
	void configurePass(OtComputePass& pass) override {
		// initialize pipeline (if required)
		if (!pipeline.isValid()) {
			pipeline.setShader(OtFogComp, sizeof(OtFogComp));
		}

		// add overlay texture
		pass.addInputSampler(depthSampler, depthTexture);

		// set uniforms
		struct Uniforms {
			glm::mat4 invProj;
			glm::vec3 fogColor;
			float fogDensity;
			float backgroundFogRatio;
		} uniforms {
			invProj,
			fogColor,
			fogDensity,
			backgroundFogRatio
		};

		pass.addUniforms(&uniforms, sizeof(uniforms));
	}

private:
	// properties
	OtTexture depthTexture;
	glm::mat4 invProj{1.0f};
	glm::vec3 fogColor{1.0f};
	float fogDensity = 1.0f;
	float backgroundFogRatio = 1.0f;
	OtSampler depthSampler{OtSampler::Filter::nearest, OtSampler::Addressing::clamp};
};
