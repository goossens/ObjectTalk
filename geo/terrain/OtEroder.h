//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <cstdint>

#include "OtFilter.h"
#include "OtShaders.h"


//
//	OtEroder
//

class OtEroder : public OtFilter {
public:
	// configure the compute pass
	void configurePass(OtComputePass& pass) override {
		// initialize pipeline (if required)
		if (!pipeline.isValid()) {
			pipeline.setShader(OtEroderComp, OtEroderCompSize);
		}

		// set uniforms
		struct Uniforms {
			glm::vec4 rounding;
			glm::vec4 onset;
			glm::vec2 assumedSlope;
			glm::vec2 texelSize;
			float scale;
			float strength;
			float gullyWeight;
			float detail;
			float cellScale;
			float gain;
			float lacunarity;
			float normalization;
			int octaves;
		} uniforms {
			rounding,
			onset,
			assumedSlope,
			sourceTexelSize,
			scale,
			strength,
			gullyWeight,
			detail,
			cellScale,
			gain,
			lacunarity,
			normalization,
			octaves
		};

		pass.addUniforms(&uniforms, sizeof(uniforms));
	}

private:
	// properties
	glm::vec4 rounding{0.1f, 0.0f, 0.1f, 2.0f};
	glm::vec4 onset{1.25f, 1.25f, 2.8f, 1.5f};
	glm::vec2 assumedSlope{0.7f, 1.0f};
	float scale = 0.15f;
	float strength = 0.22f;
	float gullyWeight = 0.5f;
	float detail = 1.5f;
	float cellScale = 0.7f;
	float gain = 0.5f;
	float lacunarity = 2.0f;
	float normalization = 0.5f;
	int octaves = 5;
};
