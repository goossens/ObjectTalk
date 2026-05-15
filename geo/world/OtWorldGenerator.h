//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <limits>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "glm/glm.hpp"
#include "nlohmann/json_fwd.hpp"

#include "OtHeightMap.h"
#include "OtImage.h"


//
//	OtWorldGenerator
//

class OtWorldGenerator {
public:
	// biome types
	enum class Biome {
		none,
		ocean,
		lake,
		marsh,
		ice,
		oceanShore,
		lakeShore,
		snow,
		tundra,
		bare,
		scorched,
		taiga,
		shrubland,
		temperateDesert,
		temperateRainForest,
		temperateDeciduousForest,
		grassland,
		subtropicalDesert,
		tropicalRainForest,
		tropicalSeasonalForest
	};

	// render types
	enum class RenderType {
		biomes,
		distanceToWater,
		heightMap
	};

	static constexpr const char* renderTypes[] = {
		"Biomes",
		"Distance To Water",
		"Height Landscape"
	};

	static constexpr size_t renderTypeCount = sizeof(renderTypes) / sizeof(*renderTypes);

	// constructor
	OtWorldGenerator();

	// load/save world definition
	void load(const std::string& filename);
	void save(const std::string& filename);

	// clear the world
	void clear();

	// see if world is valid
	inline bool isValid() { return world != nullptr; }

	// set properties
	inline void setSize(int value) { world->size = value; }
	inline void setSeed(int value) { world->seed = value; }
	inline void setRuggedness(float value) { world->ruggedness = value; }

	// generate entire world
	void generate();

	// render to image
	void render(OtImage& image, int size, RenderType type);

	// generate heightmap
	void generateHeightMap(OtHeightMap& heightmap, int size);

	// version management
	inline void setVersion(int v) { version = v; }
	inline int getVersion() { return version; }
	inline void incrementVersion() { version++; }

	// see if maps are identical
	inline bool operator==(OtWorldGenerator& rhs) {
		return world == rhs.world && version == rhs.version;
	}

	inline bool operator!=(OtWorldGenerator& rhs) {
		return !operator==(rhs);
	}

private:
	// local types
	struct Region {
	public:
		Region() = default;
		Region(size_t i, glm::vec2 c) : id(i), center(c) {}

		nlohmann::json serialize();
		void deserialize(nlohmann::json& data);

		size_t id = 0;
		glm::vec2 center;
		float distance = invalidValue;
		float elevation = invalidValue;
		float moisture = 0.0f;
		float temperature = 0.0f;
		Biome biome = Biome::none;
		bool ghost = false;
		bool border = false;
		bool water = false;
		bool ocean = false;
		bool lake = false;
		bool oceanshore = false;
		bool lakeshore = false;
		std::vector<size_t> corners;
		std::set<size_t> neighbors;
	};

	struct Corner {
	public:
		Corner() = default;
		Corner(size_t i, glm::vec2 p) : id(i), position(p) {}

		nlohmann::json serialize();
		void deserialize(nlohmann::json& data);

		size_t id = 0;
		glm::vec2 position;
		float distance = invalidValue;
		float elevation = invalidValue;
		std::vector<size_t> regions;
		std::vector<size_t> neighbors;
	};

	struct Landscape {
		nlohmann::json serialize();
		void deserialize(nlohmann::json& data);

		int size = 64;
		int seed = 37;
		float ruggedness = 0.4f;
		float northBias = -0.2f;
		float southBias = 0.2f;
		std::vector<Region> regions;
		std::vector<Corner> corners;
		std::vector<size_t> triangles;
		std::vector<size_t> halfedges;
		std::set<size_t> borders;
		std::set<size_t> oceans;
		std::set<size_t> lakes;
		std::set<size_t> oceanshores;
		std::set<size_t> lakeshores;
	};

	// properties
	std::shared_ptr<Landscape> world;
	int version = 0;

	// private functions to generate world
	void generateRegions();
	void generateCorners();
	void assignWater();
	void assignOceans();
	void assignLakes();
	void assignShores();
	void assignCoastalDistance();
	void assignElevation();
	void assignMoisture();
	void assignTemperature();
	void assignBiome();

	// utility functions
	inline void addRegion(float x, float y) { world->regions.emplace_back(world->regions.size(), glm::vec2(x, y)); }

	inline void addBorderRegion(float x, float y) {
		auto id = world->regions.size();
		auto& region = world->regions.emplace_back(id, glm::vec2(x, y));
		region.border = true;
		region.water = true;
		region.ocean = true;
		world->borders.insert(id);
		world->oceans.insert(id);
	}

	inline void addGhostRegion(float x, float y) {
		auto id = world->regions.size();
		auto& region = world->regions.emplace_back(id, glm::vec2(x, y));
		region.ghost = true;
		region.water = true;
		region.ocean = true;
		world->oceans.insert(id);
	}

	inline void addCorner(glm::vec2 pos) { world->corners.emplace_back(world->corners.size(), pos); }

	inline size_t triangleOfEdge(size_t e) { return e / 3; }
	inline size_t nextHalfEdge(size_t e) { return (e % 3 == 2) ? e - 2 : e + 1; }
	inline size_t prevHalfEdge(size_t e) { return (e % 3 == 0) ? e + 2 : e - 1; }

	static constexpr size_t invalidIndex = std::numeric_limits<size_t>::max();
	static constexpr float invalidValue = std::numeric_limits<float>::max();
};
