//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>
#include <cmath>

#include "ImGuiCurve.h"

#include "OtNumbers.h"

#include "OtParticle.h"


//
//	Utility functions
//

static glm::vec3 getRandomPointOnSphere() {
	float phi = static_cast<float>(OtRandom(std::numbers::pi));
	float theta = static_cast<float>(OtRandom(std::numbers::pi2));

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
	float angle = static_cast<float>(OtRandom(std::numbers::pi2));
	float radius = OtRandom(1.0f);
	return glm::vec3(std::cos(angle) * radius, 0.0f, std::sin(angle) * radius);
}


//
//	OtParticle::spawn
//

void OtParticle::spawn(const OtParticleSettings& settings) {
	// determine starting position and velocity for particle
	switch (settings.shape) {
		case OtParticleSettings::Shape::point:
			position = glm::vec3(0.0f, 0.0f, 0.0f);
			velocity = glm::vec3(0.0f, 1.0f, 0.0f);
			break;

		case OtParticleSettings::Shape::circle:
			position = getRandomPointinCircle();
			velocity = glm::normalize(glm::vec3(position.x, 1.0f, position.z));
			break;

		case OtParticleSettings::Shape::cone:
			position = glm::vec3(0.0f, 0.0f, 0.0f);
			velocity = glm::normalize(glm::vec3(OtRandom(-0.3f, 0.3f), 1.0f, OtRandom(-0.3f, 0.3f)));
			break;

		case OtParticleSettings::Shape::hemisphere:
			position = getRandomPointOnHemisphere();
			velocity = glm::normalize(position);
			break;

		case OtParticleSettings::Shape::sphere:
			position = getRandomPointOnSphere();
			velocity = glm::normalize(position);
			break;
	}

	// apply speed
	velocity *= settings.speed;

	// determine other properties
	gravity = settings.gravity;
	rotation = settings.rotation[0];
	scale = ImGui::CurveValueSmooth(0.0f, (int) settings.scale.size(), settings.scale.data());
	alpha = ImGui::CurveValueSmooth(0.0f, (int) settings.alpha.size(), settings.alpha.data());
	lifespan = OtRandom(settings.lifeSpanLow, settings.lifeSpanHigh);
	remaining = lifespan;
}


//
//	OtParticle::update
//

void OtParticle::update(const OtParticleSettings& settings) {
	velocity.y -= gravity * settings.deltatime;
	position += velocity * settings.deltatime;
	remaining -= settings.deltatime;
	age = std::clamp((lifespan - remaining) / lifespan, 0.0f, 1.0f);

	rotation = std::lerp(settings.rotation[0], settings.rotation[1], age);
	scale = ImGui::CurveValueSmooth(age, (int) settings.scale.size(), settings.scale.data());
	alpha = ImGui::CurveValueSmooth(age, (int) settings.alpha.size(), settings.alpha.data());
}
