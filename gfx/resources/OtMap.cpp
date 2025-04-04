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
#include <set>

#include "delaunator.h"

#include "OtHash.h"
#include "OtNoise.h"
#include "OtNumbers.h"

#include "OtCanvas.h"
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
	assignWater();
	assignOceans();

	incrementVersion();
}

void OtMap::render(OtFrameBuffer& framebuffer) {
	// render the map
	OtCanvas canvas;

	canvas.render(framebuffer, 1.0f, [&]() {
		auto scale = static_cast<float>(std::min(framebuffer.getWidth(), framebuffer.getHeight())) / static_cast<float>(map->size);
		canvas.scale(scale, scale);

		for (auto& region : map->regions) {
			if (region.water) {
				canvas.fillColor(0.0f, 0.0f, 1.0f, 1.0f);

			} else {
				canvas.fillColor(0.0f, 1.0f, 0.0f, 1.0f);
			}

			canvas.beginPath();
			canvas.moveTo(map->corners[region.corners[0]].position.x, map->corners[region.corners[0]].position.y);

			for (size_t i = 1; i < region.corners.size(); i++) {
				canvas.lineTo(map->corners[region.corners[i]].position.x, map->corners[region.corners[i]].position.y);
			}

			canvas.closePath();
			canvas.fill();
		}
	});
}


//
//	OtMap::generateRegions
//

void OtMap::generateRegions() {
	// create regions
	for (auto y = 0; y <= map->size; y++) {
		for (auto x = 0; x <= map->size; x++) {
			map->regions.emplace_back(
				(OtHash::toFloat(x, y, map->seed, 0) * 2.0f - 1.0f) * map->disturbance + x,
				(OtHash::toFloat(x, y, map->seed, 1) * 2.0f - 1.0f) * map->disturbance + y);
		}
	}

	// create extra border regions to ensure Voronoi cells cover the whole map area
	for (auto x = -1; x <= map->size + 1; x++) {
		map->regions.emplace_back(static_cast<float>(x), -2.0f, true);
		map->regions.emplace_back(static_cast<float>(x), static_cast<float>(map->size + 2), true);
	}

	for (auto y = 0; y <= map->size; y++) {
		map->regions.emplace_back(-2.0f, static_cast<float>(y), true);
		map->regions.emplace_back(static_cast<float>(map->size + 2), static_cast<float>(y), true);
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

	// save results
	map->triangles = delaunator.triangles;
	map->halfedges = delaunator.halfedges;

	// get corners for each region
	auto numEdges = map->halfedges.size();
	std::set<size_t> seen;

	for (size_t edge = 0; edge < numEdges; edge++) {
		auto region = map->triangles[nextHalfedge(edge)];

		if (seen.find(region) == seen.end()) {
			seen.insert(region);
			auto incoming = edge;

			do {
				map->regions[region].corners.emplace_back(triangleOfEdge(incoming));
				auto outgoing = nextHalfedge(incoming);
				incoming = map->halfedges[outgoing];
			} while (incoming != delaunator::INVALID_INDEX && incoming != edge);
		}
	}
}


//
//	OtMap::generateCorners
//

void OtMap::generateCorners() {
	// determine corners
	auto size = static_cast<float>(map->size);
	size_t numTriangles = map->triangles.size() / 3;

	for (size_t c = 0; c < numTriangles; c++) {
		// determine position
		glm::vec2 sum(0.0f);

		for (size_t i = 0; i < 3; i++) {
			sum += map->regions[map->triangles[3 * c + i]].center;
		}

		// create a new corner
		auto pos = sum / 3.0f;
		map->corners.emplace_back(pos, pos.x < 0.0f || pos.y < 0.0f || pos.x > size || pos.y > size);
	}
}


//
//	OtMap::assignWater
//

void OtMap::assignWater() {
	OtNoise noise;
	auto size = static_cast<float>(map->size);
	auto size2 = size / 2.0f;

	for (auto& region : map->regions) {
		if (region.border) {
			region.water = true;

		} else {
			auto x = (region.center.x - size2) / size2;
			auto y = (region.center.y - size2) / size2;
			auto distance = std::max(std::abs(x), std::abs(y));
			auto n = std::lerp(noise.fbm(x, y, static_cast<float>(map->seed)), 0.5f, 0.5f);
			region.water = (n - (0.5f * distance * distance)) < 0.0f;
		}
	}
}


//
//	OtMap::assignOceans
//

void OtMap::assignOceans() {
	for (auto& region : map->regions) {
		if (region.border) {
			region.ocean = true;

		} else {
			auto size = static_cast<float>(map->size);
			region.ocean = false;

			for (auto corner : region.corners) {
				auto pos = map->corners[corner].position;
				region.ocean |= pos.x < 0.0f || pos.y < 0.0f || pos.x > size || pos.y > size;
			}

			region.water |= region.ocean;
		}
	}
}


//
//	OtMap::getPolygon
//

void OtMap::getPolygon(size_t start, std::vector<glm::vec2>& p) {
	p.clear();
	auto incoming = start;

	do {
		p.emplace_back(map->corners[triangleOfEdge(incoming)].position);
		auto outgoing = nextHalfedge(incoming);
		incoming = map->halfedges[outgoing];
	} while (incoming != delaunator::INVALID_INDEX && incoming != start);
}
