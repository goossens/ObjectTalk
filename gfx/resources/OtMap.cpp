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
#include "OtLog.h"
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

void OtMap::update(int seed, int size, float ruggedness) {
	// create new map definition
	map = std::make_shared<Map>();
	map->seed = seed;
	map->size = size;
	map->ruggedness = ruggedness;

	generateRegions();
	triangulate();
	generateCorners();
	assignWater();
	assignOceans();
	assignLakes();
	assignShores();
	assignElevation();
	assignMoisture();
	assignTemperature();
	assignBiome();

	incrementVersion();
}

void OtMap::render(OtFrameBuffer& framebuffer) {
	static const char* colors[] = {
		"#000000",
		"#44447a",
		"#336699",
		"#2f6666",
		"#99ffff",
		"#a09077",
		"#306090",
		"#ffffff",
		"#bbbbaa",
		"#888888",
		"#555555",
		"#99aa77",
		"#889977",
		"#c9d29b",
		"#448855",
		"#679459",
		"#88aa55",
		"#d2b98b",
		"#337755",
		"#559944"
	};

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

			} else if (region.oceanshore) {
				canvas.fillColor("#a09077");

			} else {
				canvas.fillColor(colors[static_cast<size_t>(region.biome)]);
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
			addRegion(
				(OtHash::toFloat(step * x, step * y, map->seed, 0) * 2.0f - 1.0f) * map->ruggedness + x,
				(OtHash::toFloat(step * x, step * y, map->seed, 1) * 2.0f - 1.0f) * map->ruggedness + y);
		}
	}

	// create border regions
	for (auto x = 0; x <= map->size; x++) {
		addBorderRegion(step * static_cast<float>(x), 0.0f);
		addBorderRegion(step * static_cast<float>(x), step * static_cast<float>(map->size + 1));
	}

	for (auto y = 1; y < map->size; y++) {
		addBorderRegion(0.0f, step * static_cast<float>(y));
		addBorderRegion(step * static_cast<float>(map->size + 1), step * static_cast<float>(y));
	}

	// create ghost regions
	for (auto x = -1; x <= map->size + 1; x++) {
		addGhostRegion(step * static_cast<float>(x), -10.0f);
		addGhostRegion(step * static_cast<float>(x), step * static_cast<float>(map->size + 11));
	}

	for (auto y = 0; y <= map->size; y++) {
		addGhostRegion(step * -10.0f, step * static_cast<float>(y));
		addGhostRegion(step * static_cast<float>(map->size + 11), step * static_cast<float>(y));
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
		addCorner(sum / 3.0f);
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
				auto corner = triangleOfEdge(incoming);
				map->regions[region].corners.emplace_back(corner);
				map->corners[corner].regions.emplace_back(region);

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
	noise.setStartFrequency(0.5f + 3.0f * map->ruggedness);

	auto size = static_cast<float>(map->size);
	auto size2 = size / 2.0f;

	for (auto& region : map->regions) {
		if (!region.ghost && !region.border) {
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
		stack.push(region);
	}

	while (!stack.empty()) {
		auto& region = map->regions[stack.top()];
		stack.pop();

		for (auto n : region.neighbors) {
			auto& neighbor = map->regions[n];

			// expand search to other watery neighbors that have not been marked as oceans yet
			if (neighbor.water && !neighbor.ocean) {
				neighbor.ocean = true;
				map->oceans.insert(neighbor.id);
				stack.push(neighbor.id);
			}
		}
	}
}


//
//	OtMap::assignLakes
//

void OtMap::assignLakes() {
	for (auto& region : map->regions) {
		if (region.water && !region.ocean) {
			region.lake = true;
			map->lakes.insert(region.id);
		}
	}
}


//
//	OtMap::assignShores
//

void OtMap::assignShores() {
	for (auto& region : map->regions) {
		if (!region.water) {
			for (auto neighbor : region.neighbors) {
				if (map->regions[neighbor].ocean) {
					region.oceanshore = true;
					map->oceanshores.insert(neighbor);

				} else if (map->regions[neighbor].lake) {
					region.lakeshore = true;
					map->lakeshores.insert(neighbor);
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

	// process all ocean shoreline regions
	std::list<size_t> list;

	for (auto shore : map->oceanshores) {
		map->regions[shore].elevation = 0.1f;
		list.push_back(shore);
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
					list.push_back(neighbor.id);
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

	// assign elevations to corners
	for (auto& corner : map->corners) {
		if (corner.regions.size()) {
			for (auto region : corner.regions) {
				corner.elevation += map->regions[region].elevation;
			}

			corner.elevation /= corner.regions.size();
		} else {
			OtLogDebug("test");
		}
	}
}


//
//	OtMap::assignMoisture
//

void OtMap::assignMoisture() {
	// process all water regions
	for (auto& region : map->regions) {
		if (region.water) {
			region.moisture = 1.0f;
		}
	}

	// process all shoreline regions
	std::list<size_t> list;

	for (auto shore : map->oceanshores) {
		map->regions[shore].moisture = 0.9f;
		list.push_back(shore);
	}

	for (auto shore : map->lakeshores) {
		map->regions[shore].moisture = 0.9f;
		list.push_back(shore);
	}

	// process all other regions
	while (!list.empty()) {
		auto& region = map->regions[list.front()];
		list.pop_front();

		auto newMoisture = region.moisture - 0.2f;

		for (auto n : region.neighbors) {
			auto& neighbor = map->regions[n];

			if (neighbor.moisture == 0.0f || newMoisture > neighbor.moisture) {
				neighbor.moisture = newMoisture;
				list.push_back(neighbor.id);
			}
		}
	}

	// find lowest moisture level
	float minMoisture = 0.0f;

	for (auto& region : map->regions) {
		minMoisture = std::min(minMoisture, region.moisture);
	}

	// normalize moisture levels
	for (auto& region : map->regions) {
		if (!region.ocean) {
			region.moisture = (region.moisture - minMoisture) / (1.0f - minMoisture);
		}
	}
}


//
//	OtMap::assignTemperature
//

void OtMap::assignTemperature() {
	auto size = static_cast<float>(map->size);

	for (auto& region : map->regions) {
		auto latitude = region.center.y / size;
		auto bias = std::lerp(map->northBias, map->southBias, latitude);
		region.temperature = 1.0 - region.elevation + bias;
	}
}


//
//	OtMap::assignBiome
//

void OtMap::assignBiome() {
	for (auto& region : map->regions) {
		if (region.ocean) {
			region.biome = Biome::ocean;

		} else if (region.water) {
			if (region.temperature > 0.9f) {
				region.biome = Biome::marsh;

			} else if (region.temperature < 0.2f) {
				region.biome = Biome::ice;

			} else {
				region.biome = Biome::lake;

			}

		} else if (region.oceanshore) {
			region.biome = Biome::oceanShore;


		} else if (region.lakeshore) {
			region.biome = Biome::lakeShore;

		} else if (region.temperature < 0.2f) {
			if (region.moisture > 0.50f) {
				region.biome = Biome::snow;

			} else if (region.moisture > 0.33f) {
				region.biome = Biome::tundra;

			} else if (region.moisture > 0.16f) {
				region.biome = Biome::bare;

			} else {
				region.biome = Biome::scorched;
			}

		} else if (region.temperature < 0.4f) {
			if (region.moisture > 0.66f) {
				region.biome = Biome::taiga;

			} else if (region.moisture > 0.33f) {
				region.biome = Biome::shrubland;

			} else {
				region.biome = Biome::temperateDesert;
			}

		} else if (region.temperature < 0.7f) {
			if (region.moisture > 0.83f) {
				region.biome = Biome::temperateRainForest;

			} else if (region.moisture > 0.50f) {
				region.biome = Biome::temperateDeciduousForest;

			} else if (region.moisture > 0.16f) {
				region.biome = Biome::grassland;

			} else {
				region.biome = Biome::temperateDesert;
			}

		} else {
			if (region.moisture > 0.66f) {
				region.biome = Biome::tropicalRainForest;

			} else if (region.moisture > 0.33f) {
				region.biome = Biome::tropicalSeasonalForest;

			} else if (region.moisture > 0.16f) {
				region.biome = Biome::grassland;

			} else {
				region.biome = Biome::subtropicalDesert;
			}
		}
	}
}
