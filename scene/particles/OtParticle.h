//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "glm/glm.hpp"

#include "OtParticleSettings.h"


//
//	OtParticle
//

struct OtParticle {
	// constructors
	OtParticle() : lifespan(-1.0f) {}

	OtParticle(glm::vec3 p, glm::vec3 v, float g, float r, float s, float l) :
		position(p), velocity(v), gravity(g), rotation(r), scale(s), lifespan(l) {}

	// spawn a new particle based on the provided settings
	void spawn(const OtParticleSettings& settings);

	// update the particle
	void update(const OtParticleSettings& settings);

	// properties
	glm::vec3 position;
	glm::vec3 velocity;
	float gravity;
	float rotation;
	float scale;
	float alpha;
	float lifespan;
	float remaining = -1.0f;
	float age;
};
