//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>

#include "OtParticles.h"


//
//	getTextureOffset
//

static glm::vec2 getTextureOffset(int index, int rows, int columns) {
	int row = index / columns;
	int column = index % columns;

	return glm::vec2(
		static_cast<float>(column) / static_cast<float>(columns),
		static_cast<float>(row) / static_cast<float>(rows));
}


//
//	OtParticles::update
//

void OtParticles::update(const OtParticleSettings& settings) {
	// update number of particles (if required)
	if (particles.size() != settings.particles) {
		particles.resize(settings.particles);
		index.resize(settings.particles);
		instances.resize(settings.particles);
	}

	// re-spawn expired particles and update others
	for (auto& particle : particles) {
		if (particle.remaining < 0.0f) {
			particle.spawn(settings);

		} else {
			particle.update(settings);
		}
	}

	// if this is the first run, distribute remaining life and run simulation
	auto size = particles.size();

	if (first) {
		first = false;

		for (size_t i = 0; i < size; i++) {
			particles[i].remaining = settings.lifeSpanHigh * static_cast<float>(i) / static_cast<float>(size);
		}

		for (size_t i = size >> 1; i < size; i++) {
			update(settings);
		}
	}

	// build index and sort by distance
	for (size_t i = 0; i < size; i++) {
		index[i] = {i, glm::length(settings.cameraPosition - particles[i].position)};
	}

	std::sort(index.begin(), index.end(), [](const Index& i1, const Index& i2) {
		return i1.distance > i2.distance;
	});

	// create ordered instance data
	for (size_t i = 0; i < size; i++) {
		// determine texture coordinates
		auto& particle = particles[index[i].idx];

		int stages =  settings.atlasRows * settings.atlasColumns;
		float progress = particle.age * stages;

		int index1 = (int) std::floor(progress);
		int index2 = index1 < (stages - 1) ? index1 + 1 : index1;
		float blend = std::remainder(progress, 1.0f);

		glm::vec2 uv1 = getTextureOffset(index1, settings.atlasRows, settings.atlasColumns);
		glm::vec2 uv2 = getTextureOffset(index2, settings.atlasRows, settings.atlasColumns);

		// create this instance
		instances[i] = {
			particle.position,
			particle.alpha,
			uv1,
			uv2,
			particle.scale,
			glm::radians(particle.rotation),
			static_cast<float>(settings.atlasRows * 100 + settings.atlasColumns),
			blend
		};
	}
}
