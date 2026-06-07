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


//
//	OtErosion2
//

class OtErosion2 {
public:
	// configuration options
	struct HeightConfig {
		int octaves = 3;
		float gain = 0.1f;
		float lacunarity = 2.0f;
		float amplitude = 0.125f;
		float frequency = 3.0f;
		glm::vec2 offset{-0.65f, 0.0f};
	};

	struct ErosionConfig {
		float scale = 0.15f;
		float strength = 0.22f;
		float gullyWeight = 0.5f;
		float detail = 1.5f;
		glm::vec4 rounding{0.1f, 0.0f, 0.1f, 2.0f};
		glm::vec4 onset{1.25f, 1.25f, 2.8f, 1.5f};
		glm::vec2 assumedSlope{0.7f, 1.0f};
		float cellScale = 0.7f;
		int octaves = 5;
		float gain = 0.5f;
		float lacunarity = 2.0f;
		float normalization = 0.5f;
	};

	struct Erosion {
		float height;
		glm::vec2 slope;
		float magnitude;
		float ridgeMap;
	};

	// get the height and ridge map for any given point
	glm::vec2 sample(glm::vec2 position, HeightConfig& height, ErosionConfig& erosion, int seed=1337);

	// generates a raw erosion sample at a given point
	Erosion erosionFilter(glm::vec2 position, glm::vec3 heightAndSlope, float fadeTarget, ErosionConfig& erosion, int seed=1337);

private:
	// local support functions
	static glm::vec3 fractalNoise(glm::vec2 position, float frequency, int octaves, float lacunarity, float gain, int seed);
	static glm::vec4 phacelleNoise(glm::vec2 position, glm::vec2 normDir, float freq, float offset, float normalization, int seed);
	static glm::vec3 noised(int seed, glm::vec2 position);
	static glm::vec2 hash2(glm::vec2 x, int seed);
	static float smoothStart(float t, float smoothing);
	static float powInv(float t, float power);
	static float easeOut(float t);
};
