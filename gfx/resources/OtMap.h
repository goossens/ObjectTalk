//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <algorithm>
#include <cstdint>
#include <functional>
#include <limits>
#include <memory>
#include <set>
#include <vector>

#include "glm/glm.hpp"

#include "OtFrameBuffer.h"
#include "OtGeometry.h"
#include "OtImage.h"
#include "OtShaderProgram.h"


//
//	OtMap
//

class OtMap {
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

	// clear the map
	void clear();

	// see if map is valid
	inline bool isValid() { return map != nullptr; }

	// update map
	void update(int seed, int size, float ruggedness);

	// render to image
	void render(OtImage& image, int size, bool biome);

	// render map to framebuffer
	void renderHeightMap(OtFrameBuffer& framebuffer, int size);

	// convert to geometry
	OtGeometry createGeometry(float scale);

	// version management
	inline void setVersion(int v) { version = v; }
	inline int getVersion() { return version; }
	inline void incrementVersion() { version++; }

	// see if maps are identical
	inline bool operator==(OtMap& rhs) {
		return map == rhs.map && version == rhs.version;
	}

	inline bool operator!=(OtMap& rhs) {
		return !operator==(rhs);
	}

	private:
	// local types
	class Region {
	public:
		Region(size_t i, glm::vec2 c) : id(i), center(c) {}

		size_t id;
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

	class Corner {
	public:
		Corner(size_t i, glm::vec2 p) : id(i), position(p) {}

		size_t id;
		glm::vec2 position;
		float distance = invalidValue;
		float elevation = invalidValue;
		std::vector<size_t> regions;
		std::vector<size_t> neighbors;
	};

	class Map {
	public:
		int seed;
		int size;
		float ruggedness;
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
	std::shared_ptr<Map> map;
	int version = 0;

	// rendering support
	OtShaderProgram program = OtShaderProgram("OtMapVS", "OtMapFS");

	// private functions to generate map
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
	void renderImage(OtFrameBuffer& framebuffer, int size, std::function<uint32_t(float elevation)> callback);

	inline void addRegion(float x, float y) { map->regions.emplace_back(map->regions.size(), glm::vec2(x, y)); }

	inline void addGhostRegion(float x, float y) {
		auto id = map->regions.size();
		auto& region = map->regions.emplace_back(id, glm::vec2(x, y));
		region.ghost = true;
		region.water = true;
		region.ocean = true;
		map->oceans.insert(id);
	}

	inline void addBorderRegion(float x, float y) {
		auto id = map->regions.size();
		auto& region = map->regions.emplace_back(id, glm::vec2(x, y));
		region.border = true;
		region.water = true;
		region.ocean = true;
		map->borders.insert(id);
		map->oceans.insert(id);
	}

	inline void addCorner(glm::vec2 pos) { map->corners.emplace_back(map->corners.size(), pos); }

	inline size_t triangleOfEdge(size_t e) { return e / 3; }
	inline size_t nextHalfEdge(size_t e) { return (e % 3 == 2) ? e - 2 : e + 1; }
	inline size_t prevHalfEdge(size_t e) { return (e % 3 == 0) ? e + 2 : e - 1; }

	static constexpr size_t invalidIndex = std::numeric_limits<size_t>::max();
	static constexpr float invalidValue = std::numeric_limits<float>::max();
};
