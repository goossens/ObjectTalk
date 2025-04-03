//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>

#include "delaunator.h"

#include "OtHash.h"
#include "OtNoise.h"

#include "OtMap.h"


//
//	OtMap::clear
//

void OtMap::clear() {
	map = nullptr;
	incrementVersion();
}


//
//	OtMap::update
//

void OtMap::update(int seed, int size, float disturbance) {
	// create new map definition
	map = std::make_shared<Map>();
	map->seed = seed;
	map->size = size;
	map->disturbance = disturbance;

	generateRegions();
	triangulate();
	generateCorners();
	incrementVersion();
}


//
//	OtMap::generateRegions
//

void OtMap::generateRegions() {
	// create regions
	for (auto y = 0; y <= map->size; y++) {
		map->regions.emplace_back(-1.0f, static_cast<float>(y));

		for (auto x = 0; x <= map->size; x++) {
			map->regions.emplace_back(
				(OtHash::toFloat(x, y, map->seed, 0) * 2.0f - 1.0f) * map->disturbance + x,
				(OtHash::toFloat(x, y, map->seed, 1) * 2.0f - 1.0f) * map->disturbance + y);
		}

		map->regions.emplace_back(static_cast<float>(map->size + 1), static_cast<float>(y));
	}

	for (auto x = -1; x <= map->size + 1; x++) {
		map->regions.emplace_back(static_cast<float>(x), -1.0f);
		map->regions.emplace_back(static_cast<float>(x), static_cast<float>(map->size + 1));
	}
}


//
//	OtMap::triangulate
//

void OtMap::triangulate() {
	// perform Delaunay triangulation
	std::vector<double> coords;

	for (auto& region : map->regions) {
		coords.emplace_back(region.center.x);
		coords.emplace_back(region.center.y);
	}

	delaunator::Delaunator delaunator(coords);
	map->triangles = delaunator.triangles;
	map->halfedges = delaunator.halfedges;
}


//
//	OtMap::generateCorners
//

void OtMap::generateCorners() {
	// determine corners
	size_t numTriangles = map->triangles.size() / 3;

	for (size_t c = 0; c < numTriangles; c ++) {
		// determine position
		glm::vec2 sum(0.0f);

		for (size_t i = 0; i < 3; i++) {
			sum += map->regions[map->triangles[3 * c + i]].center;
		}

		auto pos = sum / 3.0f;

		// create a new corner
		auto& corner = map->corners.emplace_back(pos);

		// determine corner height
		OtNoise noise;
		auto nx = corner.position.x / map->size - 0.5f;
		auto ny = corner.position.y / map->size - 0.5f;

		auto h = (1.0f + noise.perlin(nx / 0.5f, ny / 0.5f, map->seed)) / 2.0f;
		auto d = std::max(std::abs(nx), std::abs(ny)) * 2.0f;
		corner.height = (1.0f + h - d) / 2.0f;

		// determine moisture level
		corner.moisture = (1.0f + noise.perlin(nx / 0.5f, ny / 0.5f, map->seed * 7)) / 2.0f;

		// determine status
		auto size = static_cast<float>(map->size);

		if (pos.x < 0.0f || pos.x > size || pos.y < 0.0f || pos.y > size) {
			corner.border = true;
			corner.water = true;
			corner.ocean = true;

		} else {
			corner.water = !isIsland(pos);
		}
	}
}


//
//	OtMap::isIsland
//

bool OtMap::isIsland(glm::vec2 pos) {
	static constexpr float threshold = 0.075f;
	auto size = static_cast<float>(map->size);
	auto minV = size * threshold;
	auto maxV = size * (1 - threshold);

	if (pos.x < minV || pos.y < minV || pos.x > maxV || pos.y > maxV) {
		return false;

	} else {
		OtNoise noise;
		pos -= glm::vec2(size / 2.0f, size / 2.0f);
		auto x = (pos.x / size) * 4.0f;
		auto y = (pos.y / size) * 4.0f;
		auto radius = (pos / size).length();
		return noise.perlin(x, y) >= 0.3f * radius + (radius - 0.5f);
	}
}
