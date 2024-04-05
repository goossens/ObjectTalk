//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cmath>

#include "OtNumbers.h"

#include "OtParticle.h"


//
//	Utility functions
//

static glm::vec3 getRandomPointOnSphere() {
	float phi = OtRandom(std::numbers::pi);
	float theta = OtRandom(std::numbers::pi2);

	auto x = std::sin(theta) * std::sin(phi);
	auto y = std::cos(phi);
	auto z = std::sin(theta) * std::cos(phi);

	return glm::vec3(x, y, z);
}

static glm::vec3 getRandomPointOnHemisphere() {
	glm::vec3 position = getRandomPointOnSphere();
	position.y = std::abs(position.y);
	return position;
}

static glm::vec3 getRandomPointinCircle() {
	float angle = OtRandom(std::numbers::pi2);
	float radius = OtRandom(1.0f);
	return glm::vec3(std::cos(angle) * radius, 0.0f, std::sin(angle) * radius);
}

//
//	OtParticle::spawn
//

void OtParticle::spawn(const OtParticleSettings& settings) {
	// determine starting point for particle
	switch (settings.shape) {
		case OtParticleSettings::pointShape:
			position = glm::vec3(0.0f, 0.1f, 0.0f);
			break;

		case OtParticleSettings::sphereShape:
			position = getRandomPointOnSphere();
			break;

		case OtParticleSettings::hemisphereShape:
			position = getRandomPointOnHemisphere();
			break;

		case OtParticleSettings::circleShape:
			position = getRandomPointinCircle();
			break;
	}

	// determine starting velocity
	float variation = settings.upVariation;

	velocity = settings.direction == OtParticleSettings::upDirection
		? glm::vec3(OtRandom(-variation, variation), 1.0f, OtRandom(-variation, variation))
		: glm::normalize(position);

	velocity *= settings.velocity;

	// determine other properties
	gravity = settings.gravity;
	rotation = settings.rotation[0];
	scale = settings.scale[0];
	alpha = settings.alpha[0];
	lifespan = OtRandom(settings.lifeSpan[0], settings.lifeSpan[1]);
	remaining = lifespan;
}


//
//	OtParticle::update
//

void OtParticle::update(const OtParticleSettings& settings) {
	velocity.y -= gravity * settings.deltatime;
	position += velocity * settings.deltatime;
	remaining -= settings.deltatime;
	age = (lifespan - remaining) / lifespan;

	rotation = std::lerp(settings.rotation[0], settings.rotation[1], age);
	scale = std::lerp(settings.scale[0], settings.scale[1], age);
	alpha = std::lerp(settings.alpha[0], settings.alpha[1], age);
}
