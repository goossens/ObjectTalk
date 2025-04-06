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
#include <list>
#include <stack>

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

void OtMap::update(int seed, int size, float smoothness) {
	// create new map definition
	map = std::make_shared<Map>();
	map->seed = seed;
	map->size = size;
	map->smoothness = smoothness;

	generateRegions();
	triangulate();
	generateCorners();
	assignWater();
	assignOceans();
	assignCoasts();
	assignElevation();

	incrementVersion();
}

void OtMap::render(OtFrameBuffer& framebuffer) {
	// render the map
	OtCanvas canvas;

	canvas.render(framebuffer, 1.0f, [&]() {
		auto scale = static_cast<float>(std::min(framebuffer.getWidth(), framebuffer.getHeight())) / static_cast<float>(map->size);
		canvas.scale(scale, scale);

		for (auto& region : map->regions) {
			if (region.ghost) {
				canvas.fillColor(1.0f, 0.0f, 0.0f, 1.0f);

			} else if (region.ocean) {
				canvas.fillColor("#44447a");

			} else if (region.water) {
				canvas.fillColor("#336699");

			} else if (region.coast) {
				canvas.fillColor("#a09077");

			} else {
				canvas.fillColor(0.0f, 0.2f + (region.elevation * 0.8f), 0.0f, 1.0f);
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
	auto step = static_cast<float>(map->size) / static_cast<float>(map->size + 1);

	// create internal regions
	for (auto y = 1; y < map->size; y++) {
		for (auto x = 1; x < map->size; x++) {
			map->regions.emplace_back(
				(OtHash::toFloat(step * x, step * y, map->seed, 0) * 2.0f - 1.0f) * (1.0f - map->smoothness) + x,
				(OtHash::toFloat(step * x, step * y, map->seed, 1) * 2.0f - 1.0f) * (1.0f - map->smoothness) + y);
		}
	}

	// create border regions
	for (auto x = 0; x <= map->size; x++) {
		map->borders.insert(map->regions.size());
		map->regions.emplace_back(step * static_cast<float>(x), 0.0f, false, true);
		map->borders.insert(map->regions.size());
		map->regions.emplace_back(step * static_cast<float>(x), step * static_cast<float>(map->size + 1), false, true);
	}

	for (auto y = 1; y < map->size; y++) {
		map->borders.insert(map->regions.size());
		map->regions.emplace_back(0.0f, step * static_cast<float>(y), false, true);
		map->borders.insert(map->regions.size());
		map->regions.emplace_back(step * static_cast<float>(map->size + 1), step * static_cast<float>(y), false, true);
	}

	// create ghost regions
	for (auto x = -1; x <= map->size + 1; x++) {
		map->ghosts.insert(map->regions.size());
		map->regions.emplace_back(step * static_cast<float>(x), -2.0f, true);
		map->ghosts.insert(map->regions.size());
		map->regions.emplace_back(step * static_cast<float>(x), step * static_cast<float>(map->size + 3), true);
	}

	for (auto y = 0; y <= map->size; y++) {
		map->ghosts.insert(map->regions.size());
		map->regions.emplace_back(step * -2.0f, step * static_cast<float>(y), true);
		map->ghosts.insert(map->regions.size());
		map->regions.emplace_back(step * static_cast<float>(map->size + 3), step * static_cast<float>(y), true);
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
}


//
//	OtMap::generateCorners
//

void OtMap::generateCorners() {
	// determine corners
	size_t numTriangles = map->triangles.size() / 3;

	for (size_t c = 0; c < numTriangles; c++) {
		// determine position
		glm::vec2 sum(0.0f);

		for (size_t i = 0; i < 3; i++) {
			sum += map->regions[map->triangles[3 * c + i]].center;
		}

		// create a new corner
		map->corners.emplace_back(sum / 3.0f);
	}

	// get corners for all regions
	auto numEdges = map->halfedges.size();
	std::set<size_t> seen;

	for (size_t edge = 0; edge < numEdges; edge++) {
		auto region = map->triangles[nextHalfEdge(edge)];

		if (seen.find(region) == seen.end()) {
			seen.insert(region);
			auto incoming = edge;

			do {
				map->regions[region].corners.emplace_back(triangleOfEdge(incoming));
				auto outgoing = nextHalfEdge(incoming);
				incoming = map->halfedges[outgoing];
			} while (incoming != delaunator::INVALID_INDEX && incoming != edge);
		}
	}

	// mark neighbors of all regions
	for (auto i = map->triangles.begin(); i < map->triangles.end();) {
		auto r1 = *i++;
		auto r2 = *i++;
		auto r3 = *i++;

		map->regions[r1].neighbors.insert(r2);
		map->regions[r1].neighbors.insert(r3);
		map->regions[r2].neighbors.insert(r1);
		map->regions[r2].neighbors.insert(r3);
		map->regions[r3].neighbors.insert(r1);
		map->regions[r3].neighbors.insert(r2);
	}
}


//
//	OtMap::assignWater
//

void OtMap::assignWater() {
	OtNoise noise;
	noise.setStartFrequency(0.5f + 3.0f * (1.0f - map->smoothness));

	auto size = static_cast<float>(map->size);
	auto size2 = size / 2.0f;

	for (auto& region : map->regions) {
		if (region.ghost || region.border) {
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
	// start with the border regions since they are always oceans
	std::stack<size_t> stack;

	for (auto region : map->borders) {
		map->regions[region].ocean = true;
		map->oceans.insert(region);
		stack.push(region);
	}

	while (!stack.empty()) {
		auto& region = map->regions[stack.top()];
		stack.pop();

		for (auto n : region.neighbors) {
			auto& neighbor = map->regions[n];

			// expand search to other watery neighbors that have not been marked as ocens yet
			if (neighbor.water && !neighbor.ocean) {
				neighbor.ocean = true;
				map->oceans.insert(n);
				stack.push(n);
			}
		}
	}
}


//
//	OtMap::assignCoast
//

void OtMap::assignCoasts() {
	for (auto& region : map->regions) {
		if (!region.water) {
			for (auto neighbor : region.neighbors) {
				if (map->regions[neighbor].ocean) {
					region.coast = true;
					map->coasts.insert(neighbor);
				}
			}
		}
	}
}


//
//	OtMap::assignElevation
//

void OtMap::assignElevation() {
	// process all ocean regions
	for (auto ocean : map->oceans) {
		map->regions[ocean].elevation = -0.1f;
	}

	// process all coastline regions
	std::list<size_t> list;

	for (auto coast : map->coasts) {
		map->regions[coast].elevation = 0.1f;
		list.push_back(coast);
	}

	// process all other land regions
	while (!list.empty()) {
		auto& region = map->regions[list.front()];
		list.pop_front();

		auto newElevation = region.elevation + (region.water ? 0.0f : 1.0f);

		for (auto n : region.neighbors) {
			auto& neighbor = map->regions[n];

			if (!neighbor.ocean) {
				if (neighbor.elevation == 0.0f || newElevation < neighbor.elevation) {
					neighbor.elevation = newElevation;
					list.push_back(n);
				}
			}
		}
	}

	// find highest elevation
	float maxElevation = 0.0f;

	for (auto& region : map->regions) {
		maxElevation = std::max(maxElevation, region.elevation);
	}

	// normalize elevations and redistribute so that lower elevations are more common than higher elevations
	for (auto& region : map->regions) {
		if (!region.ocean) {
			float normalizedElevation = (region.elevation - 0.1f) / (maxElevation - 0.1f);
			region.elevation = normalizedElevation * normalizedElevation;
		}
	}
}
