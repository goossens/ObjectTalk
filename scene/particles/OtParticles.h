//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <vector>

#include "glm/glm.hpp"

#include "OtVertex.h"

#include "OtParticle.h"
#include "OtParticleSettings.h"


//
//	OtParticles
//

class OtParticles {
public:
	// update all particles
	void update(const OtParticleSettings& settings);

	// get particle system information
	inline void* data() { return instances.data(); }
	inline size_t size() { return particles.size(); }

private:
	// list of particles
	std::vector<OtParticle> particles;

	// particle index (for sorting)
	struct Index {
		size_t idx;
		float distance;
	};

	std::vector<Index> index;

	// instance data
	std::vector<OtVertexParticle> instances;

	// flag to see if we are running update for the first time
	bool first = true;
};
