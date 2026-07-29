//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//	Based on http://www-cs-students.stanford.edu/~amitp/game-programming/polygon-map-generation/


//
//	Include files
//

#include <algorithm>
#include <cmath>
#include <list>

#include "delaunator.h"
#include "nlohmann/json.hpp"

#include "OtText.h"

#include "OtGlm.h" // IWYU pragma: keep
#include "OtNoise.h"

#include "OtWorld.h"


//
//	Biome rendering colors
//

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


//
//	Local 2D bounding box type
//

struct BoundingBox {
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
//	barycentric
//

static glm::vec3 barycentric(glm::vec2& a, glm::vec2& b, glm::vec2& c, glm::vec2& p) {
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
//	OtWorld::load
//

void OtWorld::load(const std::string& filename) {
	clear();
	auto text = OtText::load(filename);
	auto data = nlohmann::json::parse(text);
	world->deserialize(data);
}


//
//	OtWorld::save
//

void OtWorld::save(const std::string& filename) {
	auto data = world->serialize();
	OtText::save(filename, data.dump(1, '\t'));
}


//
//	OtWorld::clear
//

void OtWorld::clear() {
	world = std::make_shared<World>();
	incrementVersion();
}


//
//	OtWorld::update
//

void OtWorld::generate() {
	clear();
	world->regionGridSize = regionGridSize;
	world->seed = seed;
	world->ruggedness = ruggedness;
	world->generate();
}


//
//	OtWorld::render
//

void OtWorld::render(OtImage& image, int dimension, RenderType type) const {
	if (type == RenderType::regions) {
		renderRegions(image, dimension);

	} else if (type == RenderType::biomes) {
		renderColoredRegions(image, dimension, [](Region& region, OtImageCanvas& canvas) {
			auto color = colors[static_cast<size_t>(region.biome)];
			canvas.strokeColor(color);
			canvas.fillColor(color);
		});

	} else if (type == RenderType::distanceToWater) {
		renderColoredRegions(image, dimension, [](Region& region, OtImageCanvas& canvas) {
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
		});

	} else if (type == RenderType::heightMap) {
		renderHeightMap(image, dimension);
	}

	image.incrementVersion();
}


//
//	OtWorld::renderRegions
//

void OtWorld::renderRegions(OtImage& image, int dimension) const {
	// render the world
	OtImageCanvas canvas(dimension, dimension);
	auto offset = static_cast<float>(ghostOffset) + 0.1f;
	auto canvasScale = static_cast<float>(dimension) / (world->regionGridSize + offset * 2.0f);
	canvas.scale(canvasScale, canvasScale);
	canvas.translate(offset, offset);
	canvas.lineWidth(1.0f / canvasScale);

	// render result of triangulation
	canvas.beginPath();

	for (auto& region : world->regions) {
		for (auto& index : region.neighbors) {
			auto& neighbor = world->regions[index];
			canvas.moveTo(region.center.x, region.center.y);
			canvas.lineTo(neighbor.center.x, neighbor.center.y);
		}
	}

	canvas.strokeColor(0.4f, 0.4f, 0.4f, 1.0f);
	canvas.stroke();

	// render region outlines
	canvas.beginPath();

	for (auto& region : world->regions) {
		if (!region.ghost) {
			canvas.addShape(region.shape);
		}
	}

	canvas.strokeColor(0.0f, 0.7f, 0.0f, 1.0f);
	canvas.stroke();

	// mark extend
	canvas.beginPath();
	canvas.moveTo(0.0f, 0.0f);
	canvas.lineTo(0.0f, static_cast<float>(regionGridSize));
	canvas.lineTo(static_cast<float>(regionGridSize), static_cast<float>(regionGridSize));
	canvas.lineTo(static_cast<float>(regionGridSize), 0.0f);
	canvas.closePath();
	canvas.lineWidth(2.0f / canvasScale);
	canvas.strokeColor(0.0f, 0.7f, 0.7f, 1.0f);
	canvas.stroke();

	// mark all region centers
	for (auto& region : world->regions) {
		canvas.beginPath();
		canvas.circle(region.center.x, region.center.y, 3.0f / canvasScale);

		if (region.ghost) {
			canvas.fillColor(0.0f, 0.0f, 0.7f, 1.0f);

		} else {
			canvas.fillColor(0.7f, 0.0f, 0.0f, 1.0f);
		}

		canvas.fill();
	}

	canvas.render(image);
}


//
//	OtWorld::renderHeightMap
//

void OtWorld::renderHeightMap(OtImage& image, int dimension) const {
	// create heightmap
	OtHeightMap heightmap;
	generateHeightMap(heightmap, dimension);

	float minElevation = heightmap.getMinElevation();
	float maxElevation = heightmap.getMaxElevation();
	float range = maxElevation - minElevation;

	// get image ready
	image.update(dimension, dimension, OtImage::Format::rgba32);
	auto p = static_cast<float*>(image.getPixels());

	for (int y = 0; y < dimension; y++) {
		for (int x = 0; x < dimension; x++) {
			auto elevation = heightmap.getElevation(x, y);
			*p++ = (elevation - minElevation) / range;
			*p++ = 0.0f;
			*p++ = 0.0f;
			*p++ = 1.0f;
		}
	}
}


//
//	OtWorld::renderColoredRegions
//

void OtWorld::renderColoredRegions(OtImage& image, int dimension, std::function<void(Region&, OtImageCanvas&)> setup) const {
	OtImageCanvas canvas(dimension, dimension);
	auto scale = static_cast<float>(dimension) / world->regionGridSize;
	canvas.scale(scale, scale);
	canvas.lineWidth(1.0f / scale);

	for (auto& region : world->regions) {
		canvas.addShape(region.shape);
		setup(region, canvas);
		canvas.strokeAndFill();
	}

	canvas.render(image);
}


//
//	OtWorld::generateHeightMap
//

void OtWorld::generateHeightMap(OtHeightMap& heightmap, int dimension) const {
	// create vertex buffers
	struct Vertex {
		Vertex(glm::vec2 p, float e) : position(p), elevation(e) {}
		glm::vec2 position;
		float elevation;
	};

	std::vector<Vertex> vertices;
	auto scale = static_cast<float>(dimension) / world->regionGridSize;

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
	heightmap.update(dimension, dimension);

	// render all triangles
	for (size_t i = 0; i < indices.size(); i += 3) {
		auto& v1 = vertices[indices[i]];
		auto& v2 = vertices[indices[i + 1]];
		auto& v3 = vertices[indices[i + 2]];

		// determine bounding box
		BoundingBox bbox;
		bbox.add(static_cast<int>(v1.position.x), static_cast<int>(v1.position.y));
		bbox.add(static_cast<int>(v2.position.x), static_cast<int>(v2.position.y));
		bbox.add(static_cast<int>(v3.position.x), static_cast<int>(v3.position.y));
		bbox.clamp(0, dimension - 1, 0, dimension - 1);

		// render pixels
		for (int y = bbox.minY; y <= bbox.maxY; y++) {
			for (int x = bbox.minX; x <= bbox.maxX; x++) {
				auto pixelCenter = glm::vec2(x + 0.5f, y + 0.5f);
				auto bc = barycentric(v1.position, v2.position, v3.position, pixelCenter);

				if (bc.x > 0.0f && bc.y >= 0.0f && bc.z >= 0.0f && !std::isnan(bc.x)) {
					auto elevation = bc.x * v1.elevation + bc.y * v2.elevation + bc.z * v3.elevation;
					heightmap.setElevation(x, y, elevation);
				}
			}
		}
	}
}



//
//	OtWorld::World::serialize
//

nlohmann::json OtWorld::World::serialize() {
	auto data = nlohmann::json::object();

	data["regionGridSize"] = regionGridSize;
	data["seed"] = seed;
	data["ruggedness"] = ruggedness;
	data["northBias"] = northBias;
	data["southBias"] = southBias;
	return data;
}


//
//	OtWorld::World::deserialize
//

void OtWorld::World::deserialize(nlohmann::json& data) {
	regionGridSize = data.value("regionGridSize", 64);
	seed = data.value("seed", 37);
	ruggedness = data.value("ruggedness", 0.4f);
	northBias = data.value("northBias", -0.2f);
	southBias = data.value("southBias", 0.2f);
}


//
//	OtWorld::World::generate
//

void OtWorld::World::generate() {
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
//	OtWorld::World::generateRegions
//

void OtWorld::World::generateRegions() {
	OtNoise noise;

	// create internal regions
	for (auto y = 1; y < regionGridSize; y++) {
		for (auto x = 1; x < regionGridSize; x++) {
			addRegion(
				x + noise.noise(
					static_cast<float>(x),
					static_cast<float>(y),
					static_cast<float>(seed + 1)) * 0.48f,
				y + noise.noise(
					static_cast<float>(x),
					static_cast<float>(y),
					static_cast<float>(seed + 2)) * 0.48f);
		}
	}

	// create border regions (top and bottom)
	for (auto x = 0; x <= regionGridSize; x++) {
		addBorderRegion(static_cast<float>(x), 0.0f);
		addBorderRegion(static_cast<float>(x), static_cast<float>(regionGridSize));
	}

	// create border regions (left and right)
	for (auto y = 1; y < regionGridSize; y++) {
		addBorderRegion(0.0f, static_cast<float>(y));
		addBorderRegion(static_cast<float>(regionGridSize), static_cast<float>(y));
	}

	// create ghost regions (top and bottom)
	for (auto x = -1; x <= regionGridSize + 1; x++) {
		addGhostRegion(static_cast<float>(x), static_cast<float>(-ghostOffset));
		addGhostRegion(static_cast<float>(x), static_cast<float>(regionGridSize + ghostOffset));
	}

	// create ghost regions (left and right)
	for (auto y = 0; y <= regionGridSize; y++) {
		addGhostRegion(static_cast<float>(-ghostOffset), static_cast<float>(y));
		addGhostRegion(static_cast<float>(regionGridSize + ghostOffset), static_cast<float>(y));
	}

	// perform Delaunay triangulation on regions
	std::vector<double> coords;

	for (auto& region : regions) {
		coords.emplace_back(region.center.x);
		coords.emplace_back(region.center.y);
	}

	delaunator::Delaunator delaunator(coords);
	triangles = delaunator.triangles;
	halfedges = delaunator.halfedges;

	// mark neighbors of all regions
	for (auto i = triangles.begin(); i < triangles.end();) {
		auto r1 = *i++;
		auto r2 = *i++;
		auto r3 = *i++;

		regions[r1].neighbors.insert(r2);
		regions[r1].neighbors.insert(r3);
		regions[r2].neighbors.insert(r1);
		regions[r2].neighbors.insert(r3);
		regions[r3].neighbors.insert(r1);
		regions[r3].neighbors.insert(r2);
	}
}


//
//	OtWorld::World::generateCorners
//

void OtWorld::World::generateCorners() {
	// determine corners
	size_t numTriangles = triangles.size() / 3;

	for (size_t triangle = 0; triangle < numTriangles; triangle++) {
		// determine position
		glm::vec2 sum(0.0f);

		for (size_t vertex = 0; vertex < 3; vertex++) {
			sum += regions[triangles[3 * triangle + vertex]].center;
		}

		// create a new corner
		addCorner(sum / 3.0f);
	}

	// get corners for all regions
	auto numEdges = halfedges.size();
	std::set<size_t> seen;

	for (size_t edge = 0; edge < numEdges; edge++) {
		auto region = triangles[nextHalfEdge(edge)];

		if (!seen.contains(region)) {
			seen.insert(region);
			auto incoming = edge;

			do {
				auto corner = triangleOfEdge(incoming);
				regions[region].corners.emplace_back(corner);
				corners[corner].regions.emplace_back(region);

				auto outgoing = nextHalfEdge(incoming);
				incoming = halfedges[outgoing];
			} while (incoming != invalidIndex && incoming != edge);
		}
	}

	// get neighbors of all corners
	size_t numCorners = corners.size();

	for (size_t corner = 0; corner < numCorners; corner++) {
		for (size_t vertex = 0; vertex < 3; vertex++) {
			corners[corner].neighbors.emplace_back(3 * corner + vertex);
		}
	}

	// turn non-ghost regions into drawable shapes
	for (auto& region : regions) {
		if (!region.ghost) {
			auto& shape = region.shape;
			shape.moveTo(corners[region.corners[0]].position.x, corners[region.corners[0]].position.y);

			for (size_t i = 1; i < region.corners.size(); i++) {
				shape.lineTo(corners[region.corners[i]].position.x, corners[region.corners[i]].position.y);
			}

			shape.close();
		}
	}
}


//
//	OtWorld::assignWater
//

void OtWorld::World::assignWater() {
	OtNoise noise;
	noise.setFrequency(0.5f + 3.0f * ruggedness);

	auto size2 = static_cast<float>(regionGridSize) / 2.0f;

	for (auto& region : regions) {
		if (!region.ghost && !region.border) {
			auto x = (region.center.x - size2) / size2;
			auto y = (region.center.y - size2) / size2;
			auto distance = std::max(std::abs(x), std::abs(y));
			auto n = std::lerp(noise.fbm(x, y, static_cast<float>(seed)), 0.5f, 0.5f);
			region.water = (n - (0.5f * distance * distance)) < 0.0f;
		}
	}
}


//
//	OtWorld::assignOceans
//

void OtWorld::World::assignOceans() {
	// start with the border regions since they are always oceans
	std::stack<size_t> stack;

	for (auto region : borders) {
		stack.push(region);
	}

	while (!stack.empty()) {
		auto& region = regions[stack.top()];
		stack.pop();

		for (auto n : region.neighbors) {
			auto& neighbor = regions[n];

			// expand search to other watery neighbors that have not been marked as oceans yet
			if (neighbor.water && !neighbor.ocean) {
				neighbor.ocean = true;
				oceans.insert(neighbor.id);
				stack.push(neighbor.id);
			}
		}
	}
}


//
//	OtWorld::World::assignLakes
//

void OtWorld::World::assignLakes() {
	for (auto& region : regions) {
		if (region.water && !region.ocean) {
			region.lake = true;
			lakes.insert(region.id);
		}
	}
}


//
//	OtWorld::assignShores
//

void OtWorld::World::assignShores() {
	for (auto& region : regions) {
		if (!region.water) {
			for (auto neighbor : region.neighbors) {
				if (regions[neighbor].ocean) {
					region.oceanshore = true;
					oceanshores.insert(region.id);

				} else if (regions[neighbor].lake) {
					region.lakeshore = true;
					lakeshores.insert(region.id);
				}
			}
		}
	}
}


//
//	OtWorld::World::assignCoastalDistance
//

void OtWorld::World::assignCoastalDistance() {
	// process all ocean shoreline regions
	std::list<size_t> list;

	for (auto shore : oceanshores) {
		regions[shore].distance = 0.0f;
		list.push_back(shore);
	}

	// process all other regions
	while (!list.empty()) {
		auto& region = regions[list.front()];
		list.pop_front();

		auto newDistance = region.distance + 1.0f;

		for (auto n : region.neighbors) {
			auto& neighbor = regions[n];

			if (neighbor.distance == invalidValue || newDistance < neighbor.distance) {
				neighbor.distance = newDistance;
				list.push_back(neighbor.id);
			}
		}
	}

	// find distance limit
	float maxDistance = std::numeric_limits<float>::lowest();

	for (auto& region : regions) {
		maxDistance = std::max(maxDistance, region.distance);
	}

	// normalize distances
	for (auto& region : regions) {
		region.distance = region.distance / maxDistance;
	}

	// assign distances to corners
	for (auto& corner : corners) {
		auto total = 0.0f;

		for (auto region : corner.regions) {
			total += regions[region].distance;
		}

		corner.distance = total / static_cast<float>(corner.regions.size());
	}
}


//
//	OtWorld::World::assignElevation
//

void OtWorld::World::assignElevation() {
	// process all ocean shoreline regions
	std::list<size_t> list;

	for (auto shore : oceanshores) {
		regions[shore].elevation = 0.0f;
		list.push_back(shore);
	}

	// process all other regions
	while (!list.empty()) {
		auto& region = regions[list.front()];
		list.pop_front();

		for (auto n : region.neighbors) {
			auto& neighbor = regions[n];

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

	for (auto& region : regions) {
		if (!region.water && !region.oceanshore && !region.lakeshore) {
			auto offset = noise.fbm(region.center.x, region.center.y, static_cast<float>(seed + 7));
			region.elevation += ((offset * 2.0f) - 1.0f) * 0.25f;
		}
	}

	// find elevation limits
	float minElevation = std::numeric_limits<float>::max();
	float maxElevation = std::numeric_limits<float>::lowest();

	for (auto& region : regions) {
		minElevation = std::min(minElevation, region.elevation);
		maxElevation = std::max(maxElevation, region.elevation);
	}

	// normalize elevations and redistribute
	for (auto& region : regions) {
		if (region.ocean) {
			float normalizedElevation = region.elevation / minElevation;
			region.elevation = -std::pow(normalizedElevation, 1.5f);

		} else {
			float normalizedElevation = region.elevation / maxElevation;
			region.elevation = std::pow(normalizedElevation, 1.5f);
		}
	}

	// assign elevations to corners
	for (auto& corner : corners) {
		auto total = 0.0f;

		for (auto region : corner.regions) {
			total += regions[region].elevation;
		}

		corner.elevation = total / static_cast<float>(corner.regions.size());
	}
}


//
//	OtWorld::World::assignMoisture
//

void OtWorld::World::assignMoisture() {
	// process all water regions
	for (auto& region : regions) {
		if (region.water) {
			region.moisture = 1.0f;
		}
	}

	// process all shoreline regions
	std::list<size_t> list;

	for (auto shore : oceanshores) {
		regions[shore].moisture = 0.9f;
		list.push_back(shore);
	}

	for (auto shore : lakeshores) {
		regions[shore].moisture = 0.9f;
		list.push_back(shore);
	}

	// process all other regions
	while (!list.empty()) {
		auto& region = regions[list.front()];
		list.pop_front();

		auto newMoisture = region.moisture - 0.2f;

		for (auto n : region.neighbors) {
			auto& neighbor = regions[n];

			if (neighbor.moisture == 0.0f || newMoisture > neighbor.moisture) {
				neighbor.moisture = newMoisture;
				list.push_back(neighbor.id);
			}
		}
	}

	// find lowest moisture level
	float minMoisture = 0.0f;

	for (auto& region : regions) {
		minMoisture = std::min(minMoisture, region.moisture);
	}

	// normalize moisture levels
	for (auto& region : regions) {
		if (!region.ocean) {
			region.moisture = (region.moisture - minMoisture) / (1.0f - minMoisture);
		}
	}
}


//
//	OtWorld::World::assignTemperature
//

void OtWorld::World::assignTemperature() {
	for (auto& region : regions) {
		auto latitude = region.center.y / static_cast<float>(regionGridSize);
		auto bias = std::lerp(northBias, southBias, latitude);
		region.temperature = 1.0f - region.elevation + bias;
	}
}


//
//	OtWorld::World::assignBiome
//

void OtWorld::World::assignBiome() {
	for (auto& region : regions) {
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
