//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//	Ported from https://github.com/lpmitchell/AdvancedTerrainErosion
//	Based on https://blog.runevision.com/2026/03/fast-and-gorgeous-erosion-filter.html


//
//	Include files
//

#include "OtHash.h"

#include "OtErosion.h"


//
//	OtErosion::process
//

void OtErosion::process(const ErosionConfig& config, OtHeightMap& heightMap, size_t cycles) {
	for (size_t i = 0; i < cycles; i++) {
		Particle drop(glm::vec2(
			OtHash::toFloat(static_cast<uint32_t>(i + 17)),
			OtHash::toFloat(static_cast<uint32_t>(i + 67))));

		while (drop.volume < config.minimumVolume) {
			// get surface normal at drop
			glm::vec3 n = heightMap.sampleNormal(drop.pos.x, drop.pos.y);

			// accelerate particle using newtonian mechanics using the surface normal
			drop.speed += config.dt * glm::vec2(n.x, n.z)  / (drop.volume * config.density);
			drop.pos += config.dt * drop.speed;
			drop.speed *= 1.0 - config.dt * config.friction;
		}
	}
}
