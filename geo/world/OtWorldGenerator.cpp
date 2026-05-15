//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <list>
#include <numbers>
#include <stack>

#include "delaunator.h"
#include "glm/gtc/matrix_transform.hpp"
#include "nlohmann/json.hpp"

#include "OtText.h"

#include "OtGlm.h"
#include "OtNoise.h"
#include "OtImageCanvas.h"

#include "OtWorldGenerator.h"


//
//	Local 2D bounding box type
//

struct Bbox {
	inline void add(int x, int y) {
		minX = std::min(minX, x);
		minY = std::min(minY, y);
		maxX = std::max(maxX, x);
		maxY = std::max(maxY, y);
	}

	inline void clamp(int l, int r, int t, int b) {
		minX = std::clamp(minX, l, r);
		maxX = std::clamp(maxX, l, r);
		minY = std::clamp(minY, t, b);
		maxY = std::clamp(maxY, t, b);
	}

	int minX = std::numeric_limits<int>::max();
	int minY = std::numeric_limits<int>::max();
	int maxX = std::numeric_limits<int>::lowest();
	int maxY = std::numeric_limits<int>::lowest();
};


//
//	baryCentric
//

static glm::vec3 baryCentric(glm::vec2& a, glm::vec2& b, glm::vec2& c, glm::vec2& p) {
	auto v0 = b - a;
	auto v1 = c - a;
	auto v2 = p - a;

	float D = v0.x * v1.y - v1.x * v0.y;

	float v = (v2.x * v1.y - v1.x * v2.y) / D;
	float w = (v0.x * v2.y - v2.x * v0.y) / D;
	float u = 1.0f - v - w;

	return glm::vec3(u, v, w);
}


//
//	OtWorldGenerator::OtWorldGenerator
//

OtWorldGenerator::OtWorldGenerator() {
	world = std::make_shared<Landscape>();
}


//
//	OtWorldGenerator::load
//

void OtWorldGenerator::load(const std::string& filename) {
	clear();
	auto text = OtText::load(filename);
	auto data = nlohmann::json::parse(text);
	world->deserialize(data);
}


//
//	OtWorldGenerator::save
//

void OtWorldGenerator::save(const std::string& filename) {
	auto data = world->serialize();
	OtText::save(filename, data.dump(1, '\t'));
}


//
//	OtWorldGenerator::clear
//

void OtWorldGenerator::clear() {
	world = std::make_shared<Landscape>();
	incrementVersion();
}


//
//	OtWorldGenerator::update
//

void OtWorldGenerator::generate() {
	clear();
	generateRegions();
	generateCorners();
	assignWater();
	assignOceans();
	assignLakes();
	assignShores();
	assignCoastalDistance();
	assignElevation();
	assignMoisture();
	assignTemperature();
	assignBiome();
}


//
//	OtWorldGenerator::render
//

void OtWorldGenerator::render(OtImage& image, int size, RenderType type) {
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

	// see if we need to render a heightmap
	if (type == RenderType::heightMap) {
		// create heightmap
		OtHeightMap heightmap;
		generateHeightMap(heightmap, size);

		float minElevation = heightmap.getMinHeight();
		float maxElevation = heightmap.getMaxHeight();
		float range = maxElevation - minElevation;

		// get image ready
		image.update(size, size, OtImage::Format::rgba32);
		auto p = static_cast<float*>(image.getPixels());

		for (int y = 0; y < size; y++) {
			for (int x = 0; x < size; x++) {
				auto elevation = heightmap.getHeight(x, y);
				*p++ = (elevation - minElevation) / range;
				*p++ = 0.0f;
				*p++ = 0.0f;
				*p++ = 1.0f;
			}
		}

	} else {
		// render the world
		OtImageCanvas canvas(size, size);
		auto scale = static_cast<float>(size) / static_cast<float>(world->size);
		canvas.scale(scale, scale);

		for (auto& region : world->regions) {
			if (type == RenderType::biomes) {
				auto color = colors[static_cast<size_t>(region.biome)];
				canvas.strokeColor(color);
				canvas.fillColor(color);

			} else {
				if (region.ocean) {
					auto red = 0.25f * (1.0f + region.elevation);
					auto green = 0.25f * (1.0f + region.elevation);
					auto blue = 0.5f * (1.0f + region.elevation);
					canvas.strokeColor(red, green, blue, 1.0f);
					canvas.fillColor(red, green, blue, 1.0f);

				} else if (region.water) {
					auto color = colors[static_cast<size_t>(region.biome)];
					canvas.strokeColor(color);
					canvas.fillColor(color);

				} else {
					auto white = (1.0f - region.temperature) * (1.0f - region.temperature);
					auto moisture = 1.0f - ((1.0f - region.moisture) * (1.0f - region.moisture));
					auto red = white + (0.85f - 0.39f * moisture) * (1.0f - white);
					auto green = white + (0.73f - 0.18f * moisture) * (1.0f - white);
					auto blue = white + (0.55f - 0.18f * moisture) * (1.0f - white);
					canvas.strokeColor(red, green, blue, 1.0f);
					canvas.fillColor(red, green, blue, 1.0f);
				}
			}

			canvas.beginPath();
			canvas.moveTo(world->corners[region.corners[0]].position.x, world->corners[region.corners[0]].position.y);

			for (size_t i = 1; i < region.corners.size(); i++) {
				canvas.lineTo(world->corners[region.corners[i]].position.x, world->corners[region.corners[i]].position.y);
			}

			canvas.closePath();
			canvas.strokeAndFill();
		}

		canvas.render(image);
	}

	image.incrementVersion();
}


//
//	OtWorldGenerator::generateHeightMap
//

void OtWorldGenerator::generateHeightMap(OtHeightMap& heightmap, int size) {
	// create vertex buffers
	struct Vertex {
		Vertex(glm::vec2 p, float e) : position(p), elevation(e) {}
		glm::vec2 position;
		float elevation;
	};

	std::vector<Vertex> vertices;
	auto scale = static_cast<float>(size) / static_cast<float>(world->size);

	for (auto& corner : world->corners) {
		vertices.emplace_back(corner.position * scale, corner.elevation);
	}

	// create index buffer
	std::vector<size_t> indices;

	for (auto& region : world->regions) {
		auto center = vertices.size();
		vertices.emplace_back(region.center * scale, region.elevation);
		auto corners = region.corners.size();

		for (size_t i = 0; i < corners; i++) {
			indices.emplace_back(center);
			indices.emplace_back(region.corners[i]);
			indices.emplace_back(region.corners[(i + 1) % corners]);
		}
	}

	// resize heightmap (if required)
	heightmap.update(size, size);

	// render all triangles
	for (size_t i = 0; i < indices.size(); i += 3) {
		auto& v1 = vertices[indices[i]];
		auto& v2 = vertices[indices[i + 1]];
		auto& v3 = vertices[indices[i + 2]];

		// determine bounding box
		Bbox bbox;
		bbox.add(static_cast<int>(v1.position.x), static_cast<int>(v1.position.y));
		bbox.add(static_cast<int>(v2.position.x), static_cast<int>(v2.position.y));
		bbox.add(static_cast<int>(v3.position.x), static_cast<int>(v3.position.y));
		bbox.clamp(0, size - 1, 0, size - 1);

		// render pixels
		for (int y = bbox.minY; y <= bbox.maxY; y++) {
			for (int x = bbox.minX; x <= bbox.maxX; x++) {
				auto pixelCenter = glm::vec2(x + 0.5f, y + 0.5f);
				auto bc = baryCentric(v1.position, v2.position, v3.position, pixelCenter);

				if (bc.x > 0.0f && bc.y >= 0.0f && bc.z >= 0.0f && !std::isnan(bc.x)) {
					auto elevation = bc.x * v1.elevation + bc.y * v2.elevation + bc.z * v3.elevation;
					heightmap.setHeight(x, y, elevation);
				}
			}
		}
	}
}


//
//	OtWorldGenerator::generateRegions
//

void OtWorldGenerator::generateRegions() {
	OtNoise noise;

	// create internal regions
	for (auto y = 1; y < world->size; y++) {
		for (auto x = 1; x < world->size; x++) {
			addRegion(
				x + (noise.noise(
					static_cast<float>(x),
					static_cast<float>(y),
					static_cast<float>(world->seed + 1)) * 2.0f - 1.0f) * 0.49f,
				y + (noise.noise(
					static_cast<float>(x),
					static_cast<float>(y),
					static_cast<float>(world->seed + 2)) * 2.0f - 1.0f) * 0.49f);
		}
	}

	// create border regions (top and bottom)
	for (auto x = 0; x <= world->size; x++) {
		addBorderRegion(static_cast<float>(x), 0.0f);
		addBorderRegion(static_cast<float>(x), static_cast<float>(world->size));
	}

	// create border regions (left and right)
	for (auto y = 1; y < world->size; y++) {
		addBorderRegion(0.0f, static_cast<float>(y));
		addBorderRegion(static_cast<float>(world->size), static_cast<float>(y));
	}

	// create ghost regions (top and bottom)
	for (auto x = -1; x <= world->size + 1; x++) {
		addGhostRegion(static_cast<float>(x), -10.0f);
		addGhostRegion(static_cast<float>(x), static_cast<float>(world->size + 10.0f));
	}

	// create ghost regions (left and right)
	for (auto y = 0; y <= world->size; y++) {
		addGhostRegion(-10.0f, static_cast<float>(y));
		addGhostRegion(static_cast<float>(world->size + 10.0f), static_cast<float>(y));
	}

	// perform Delaunay triangulation on regions
	std::vector<double> coords;

	for (auto& region : world->regions) {
		coords.emplace_back(region.center.x);
		coords.emplace_back(region.center.y);
	}

	delaunator::Delaunator delaunator(coords);
	world->triangles = delaunator.triangles;
	world->halfedges = delaunator.halfedges;

	// mark neighbors of all regions
	for (auto i = world->triangles.begin(); i < world->triangles.end();) {
		auto r1 = *i++;
		auto r2 = *i++;
		auto r3 = *i++;

		world->regions[r1].neighbors.insert(r2);
		world->regions[r1].neighbors.insert(r3);
		world->regions[r2].neighbors.insert(r1);
		world->regions[r2].neighbors.insert(r3);
		world->regions[r3].neighbors.insert(r1);
		world->regions[r3].neighbors.insert(r2);
	}
}


//
//	OtWorldGenerator::generateCorners
//

void OtWorldGenerator::generateCorners() {
	// determine corners
	size_t numTriangles = world->triangles.size() / 3;

	for (size_t triangle = 0; triangle < numTriangles; triangle++) {
		// determine position
		glm::vec2 sum(0.0f);

		for (size_t vertex = 0; vertex < 3; vertex++) {
			sum += world->regions[world->triangles[3 * triangle + vertex]].center;
		}

		// create a new corner
		addCorner(sum / 3.0f);
	}

	// get corners for all regions
	auto numEdges = world->halfedges.size();
	std::set<size_t> seen;

	for (size_t edge = 0; edge < numEdges; edge++) {
		auto region = world->triangles[nextHalfEdge(edge)];

		if (seen.find(region) == seen.end()) {
			seen.insert(region);
			auto incoming = edge;

			do {
				auto corner = triangleOfEdge(incoming);
				world->regions[region].corners.emplace_back(corner);
				world->corners[corner].regions.emplace_back(region);

				auto outgoing = nextHalfEdge(incoming);
				incoming = world->halfedges[outgoing];
			} while (incoming != invalidIndex && incoming != edge);
		}
	}

	// get neighbors of all corners
	size_t numCorners = world->corners.size();

	for (size_t corner = 0; corner < numCorners; corner++) {
		for (size_t vertex = 0; vertex < 3; vertex++) {
			world->corners[corner].neighbors.emplace_back(3 * corner + vertex);
		}
	}
}


//
//	OtWorldGenerator::assignWater
//

void OtWorldGenerator::assignWater() {
	OtNoise noise;
	noise.setFrequency(0.5f + 3.0f * world->ruggedness);

	auto size = static_cast<float>(world->size);
	auto size2 = size / 2.0f;

	for (auto& region : world->regions) {
		if (!region.ghost && !region.border) {
			auto x = (region.center.x - size2) / size2;
			auto y = (region.center.y - size2) / size2;
			auto distance = std::max(std::abs(x), std::abs(y));
			auto n = std::lerp(noise.fbm(x, y, static_cast<float>(world->seed)), 0.5f, 0.5f);
			region.water = (n - (0.5f * distance * distance)) < 0.0f;
		}
	}
}


//
//	OtWorldGenerator::assignOceans
//

void OtWorldGenerator::assignOceans() {
	// start with the border regions since they are always oceans
	std::stack<size_t> stack;

	for (auto region : world->borders) {
		stack.push(region);
	}

	while (!stack.empty()) {
		auto& region = world->regions[stack.top()];
		stack.pop();

		for (auto n : region.neighbors) {
			auto& neighbor = world->regions[n];

			// expand search to other watery neighbors that have not been marked as oceans yet
			if (neighbor.water && !neighbor.ocean) {
				neighbor.ocean = true;
				world->oceans.insert(neighbor.id);
				stack.push(neighbor.id);
			}
		}
	}
}


//
//	OtWorldGenerator::assignLakes
//

void OtWorldGenerator::assignLakes() {
	for (auto& region : world->regions) {
		if (region.water && !region.ocean) {
			region.lake = true;
			world->lakes.insert(region.id);
		}
	}
}


//
//	OtWorldGenerator::assignShores
//

void OtWorldGenerator::assignShores() {
	for (auto& region : world->regions) {
		if (!region.water) {
			for (auto neighbor : region.neighbors) {
				if (world->regions[neighbor].ocean) {
					region.oceanshore = true;
					world->oceanshores.insert(region.id);

				} else if (world->regions[neighbor].lake) {
					region.lakeshore = true;
					world->lakeshores.insert(region.id);
				}
			}
		}
	}
}


//
//	OtWorldGenerator::assignCoastalDistance
//

void OtWorldGenerator::assignCoastalDistance() {
	// process all ocean shoreline regions
	std::list<size_t> list;

	for (auto shore : world->oceanshores) {
		world->regions[shore].distance = 0.0f;
		list.push_back(shore);
	}

	// process all other regions
	while (!list.empty()) {
		auto& region = world->regions[list.front()];
		list.pop_front();

		auto newDistance = region.distance + 1.0f;

		for (auto n : region.neighbors) {
			auto& neighbor = world->regions[n];

			if (neighbor.distance == invalidValue || newDistance < neighbor.distance) {
				neighbor.distance = newDistance;
				list.push_back(neighbor.id);
			}
		}
	}

	// find distance limit
	float maxDistance = std::numeric_limits<float>::lowest();

	for (auto& region : world->regions) {
		maxDistance = std::max(maxDistance, region.distance);
	}

	// normalize distances
	for (auto& region : world->regions) {
		region.distance = region.distance / maxDistance;
	}

	// assign distances to corners
	for (auto& corner : world->corners) {
		auto total = 0.0f;

		for (auto region : corner.regions) {
			total += world->regions[region].distance;
		}

		corner.distance = total / static_cast<float>(corner.regions.size());
	}
}


//
//	OtWorldGenerator::assignElevation
//

void OtWorldGenerator::assignElevation() {
	// process all ocean shoreline regions
	std::list<size_t> list;

	for (auto shore : world->oceanshores) {
		world->regions[shore].elevation = 0.0f;
		list.push_back(shore);
	}

	// process all other regions
	while (!list.empty()) {
		auto& region = world->regions[list.front()];
		list.pop_front();

		for (auto n : region.neighbors) {
			auto& neighbor = world->regions[n];

			if (neighbor.ocean) {
				auto newElevation = region.elevation - 1.0f;

				if (neighbor.elevation == invalidValue || newElevation > neighbor.elevation) {
					neighbor.elevation = newElevation;
					list.push_back(neighbor.id);
				}

			} else {
				auto newElevation = region.elevation + (neighbor.lake ? 0.0f : 1.0f);

				if (neighbor.elevation == invalidValue || newElevation < neighbor.elevation) {
					neighbor.elevation = newElevation;
					list.push_back(neighbor.id);
				}
			}
		}
	}

	// add some randomness to land elevation
	OtNoise noise;

	for (auto& region : world->regions) {
		if (!region.water && !region.oceanshore && !region.lakeshore) {
			auto offset = noise.fbm(region.center.x, region.center.y, static_cast<float>(world->seed + 7));
			region.elevation += ((offset * 2.0f) - 1.0f) * 0.25f;
		}
	}

	// find elevation limits
	float minElevation = std::numeric_limits<float>::max();
	float maxElevation = std::numeric_limits<float>::lowest();

	for (auto& region : world->regions) {
		minElevation = std::min(minElevation, region.elevation);
		maxElevation = std::max(maxElevation, region.elevation);
	}

	// normalize elevations and redistribute
	for (auto& region : world->regions) {
		if (region.ocean) {
			float normalizedElevation = region.elevation / minElevation;
			region.elevation = -std::pow(normalizedElevation, 1.5f);

		} else {
			float normalizedElevation = region.elevation / maxElevation;
			region.elevation = std::pow(normalizedElevation, 1.5f);
		}
	}

	// assign elevations to corners
	for (auto& corner : world->corners) {
		auto total = 0.0f;

		for (auto region : corner.regions) {
			total += world->regions[region].elevation;
		}

		corner.elevation = total / static_cast<float>(corner.regions.size());
	}
}


//
//	OtWorldGenerator::assignMoisture
//

void OtWorldGenerator::assignMoisture() {
	// process all water regions
	for (auto& region : world->regions) {
		if (region.water) {
			region.moisture = 1.0f;
		}
	}

	// process all shoreline regions
	std::list<size_t> list;

	for (auto shore : world->oceanshores) {
		world->regions[shore].moisture = 0.9f;
		list.push_back(shore);
	}

	for (auto shore : world->lakeshores) {
		world->regions[shore].moisture = 0.9f;
		list.push_back(shore);
	}

	// process all other regions
	while (!list.empty()) {
		auto& region = world->regions[list.front()];
		list.pop_front();

		auto newMoisture = region.moisture - 0.2f;

		for (auto n : region.neighbors) {
			auto& neighbor = world->regions[n];

			if (neighbor.moisture == 0.0f || newMoisture > neighbor.moisture) {
				neighbor.moisture = newMoisture;
				list.push_back(neighbor.id);
			}
		}
	}

	// find lowest moisture level
	float minMoisture = 0.0f;

	for (auto& region : world->regions) {
		minMoisture = std::min(minMoisture, region.moisture);
	}

	// normalize moisture levels
	for (auto& region : world->regions) {
		if (!region.ocean) {
			region.moisture = (region.moisture - minMoisture) / (1.0f - minMoisture);
		}
	}
}


//
//	OtWorldGenerator::assignTemperature
//

void OtWorldGenerator::assignTemperature() {
	auto size = static_cast<float>(world->size);

	for (auto& region : world->regions) {
		auto latitude = region.center.y / size;
		auto bias = std::lerp(world->northBias, world->southBias, latitude);
		region.temperature = 1.0f - region.elevation + bias;
	}
}


//
//	OtWorldGenerator::assignBiome
//

void OtWorldGenerator::assignBiome() {
	for (auto& region : world->regions) {
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


//
//	OtWorldGenerator::Region::serialize
//

nlohmann::json OtWorldGenerator::Region::serialize() {
	auto data = nlohmann::json::object();

	data["id"] = id;
	data["center"] = center;
	data["distance"] = distance;
	data["elevation"] = elevation;
	data["moisture"] = moisture;
	data["temperature"] = temperature;
	data["biome"] = biome;

	data["ghost"] = ghost;
	data["border"] = border;
	data["water"] = water;
	data["ocean"] = ocean;
	data["lake"] = lake;
	data["oceanshore"] = oceanshore;
	data["lakeshore"] = lakeshore;

	auto cornersData = nlohmann::json::array();
	for (auto i : corners) { cornersData.push_back(i); }
	data["corners"] = cornersData;

	auto neighborsData = nlohmann::json::array();
	for (auto i : neighbors) { neighborsData.push_back(i); }
	data["neighbors"] = neighborsData;

	return data;
}


//
//	OtWorldGenerator::Region::deserialize
//

void OtWorldGenerator::Region::deserialize(nlohmann::json& data) {
	id = data.value("id", 0);
	center = data.value("center", glm::vec2());
	distance = data.value("distance", invalidValue);
	elevation = data.value("elevation", invalidValue);
	moisture = data.value("moisture", 0.0f);
	temperature = data.value("temperature", 0.0f);
	biome = data.value("biome", Biome::none);

	ghost = data.value("ghost", false);
	border = data.value("border", false);
	water = data.value("water", false);
	ocean = data.value("ocean", false);
	lake = data.value("lake", false);
	oceanshore = data.value("oceanshore", false);
	lakeshore = data.value("lakeshore", false);

	corners.clear(); for (auto value : data["corners"]) { corners.emplace_back(value); }
	neighbors.clear(); for (auto value : data["neighbors"]) { neighbors.emplace(value); }
}


//
//	OtWorldGenerator::Corner::serialize
//

nlohmann::json OtWorldGenerator::Corner::serialize() {
	auto data = nlohmann::json::object();

	data["id"] = id;
	data["position"] = position;
	data["distance"] = distance;
	data["elevation"] = elevation;

	auto regionsData = nlohmann::json::array();
	for (auto i : regions) { regionsData.push_back(i); }
	data["regions"] = regionsData;

	auto neighborsData = nlohmann::json::array();
	for (auto i : neighbors) { neighborsData.push_back(i); }
	data["neighbors"] = neighbors;

	return data;
}


//
//	OtWorldGenerator::Corner::deserialize
//

void OtWorldGenerator::Corner::deserialize(nlohmann::json& data) {
	id = data.value("id", 0);
	position = data.value("position", glm::vec2());
	distance = data.value("distance", invalidValue);
	elevation = data.value("elevation", invalidValue);

	regions.clear(); for (auto value : data["regions"]) { regions.emplace_back(value); }
	neighbors.clear(); for (auto value : data["neighbors"]) { neighbors.emplace_back(value); }
}


//
//	OtWorldGenerator::Landscape::serialize
//

nlohmann::json OtWorldGenerator::Landscape::serialize() {
	auto data = nlohmann::json::object();

	data["size"] = size;
	data["seed"] = seed;
	data["ruggedness"] = ruggedness;
	data["northBias"] = northBias;
	data["southBias"] = southBias;

	auto regionsData = nlohmann::json::array();
	for (auto& region : regions) { regionsData.push_back(region.serialize()); }
	data["regions"] = regionsData;

	auto cornersData = nlohmann::json::array();
	for (auto& corner : corners) { cornersData.push_back(corner.serialize()); }
	data["corners"] = cornersData;

	auto trianglesData = nlohmann::json::array();
	for (auto i : triangles) { trianglesData.push_back(i); }
	data["triangles"] = trianglesData;

	auto halfedgesData = nlohmann::json::array();
	for (auto i : halfedges) { halfedgesData.push_back(i); }
	data["halfedges"] = halfedgesData;

	auto bordersData = nlohmann::json::array();
	for (auto i : borders) { bordersData.push_back(i); }
	data["borders"] = bordersData;

	auto oceansData = nlohmann::json::array();
	for (auto i : oceans) { oceansData.push_back(i); }
	data["oceans"] = oceansData;

	auto lakesData = nlohmann::json::array();
	for (auto i : lakes) { lakesData.push_back(i); }
	data["lakes"] = lakesData;

	auto oceanshoresData = nlohmann::json::array();
	for (auto i : oceanshores) { oceanshoresData.push_back(i); }
	data["oceanshores"] = oceanshoresData;

	auto lakeshoresData = nlohmann::json::array();
	for (auto i : lakeshores) { lakeshoresData.push_back(i); }
	data["lakeshores"] = lakeshoresData;

	return data;
}


//
//	OtWorldGenerator::Landscape::deserialize
//

void OtWorldGenerator::Landscape::deserialize(nlohmann::json& data) {
	size = data.value("size", 64);
	seed = data.value("seed", 37);
	ruggedness = data.value("ruggedness", 0.4f);
	northBias = data.value("northBias", -0.2f);
	southBias = data.value("southBias", 0.2f);

	regions.clear(); for (auto& value : data["regions"]) { regions.emplace_back().deserialize(value); }
	corners.clear(); for (auto& value : data["corners"]) { corners.emplace_back().deserialize(value); }
	triangles.clear(); for (auto value : data["triangles"]) { triangles.emplace_back(value); }
	halfedges.clear(); for (auto value : data["halfedges"]) { halfedges.emplace_back(value); }
	borders.clear(); for (auto value : data["borders"]) { borders.emplace(value); }
	oceans.clear(); for (auto value : data["oceans"]) { oceans.emplace(value); }
	lakes.clear(); for (auto value : data["lakes"]) { lakes.emplace(value); }
	oceanshores.clear(); for (auto value : data["oceanshores"]) { oceanshores.emplace(value); }
	lakeshores.clear(); for (auto value : data["lakeshores"]) { lakeshores.emplace(value); }
}
