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
	inline size_t size() { return particles.size(); }
	inline int getInstanceStride() { return sizeof(Instance); }
	inline void* getInstanceData () { return instances.data(); }

private:
	// list of particles
	std::vector<OtParticle> particles;

	// particle index (for sorting)
	struct Index {
		size_t idx;
		float distance;
	};

	std::vector<Index> index;

	// particle instance data
	struct Instance {
		glm::vec3 position;
		float alpha;
		glm::vec2 uv1;
		glm::vec2 uv2;
		float scale;
		float rotate;
		float grid;
		float blend;
	};

	std::vector<Instance> instances;

	// flag to see if we are running update for the first time
	bool first = true;
};
