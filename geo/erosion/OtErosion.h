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

#include "OtHeightMap.h"


//
//	OtErosion
//

class OtErosion {
public:
	struct ErosionConfig {
		float dt = 1.2f;
		float density = 1.0f;
		float evaporationRate = 0.001f;
		float depositionRate = 0.1f;
		float minimumVolume = 0.01f;
		float friction = 0.05f;
	};

	// run the specified erosion cycles on a heightmap
	void process(const ErosionConfig& config, OtHeightMap& heightMap, size_t cycles);

private:
	// single particle that started as a drop of water that carries sediment
	struct Particle{
		Particle(glm::vec2 pos) : pos(pos) {}

		glm::vec2 pos;
		glm::vec2 speed = glm::vec2(0.0f);

		float volume = 1.0f;
		float sediment = 0.0f;
	};
};
