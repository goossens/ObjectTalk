//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <memory>
#include <vector>

#include "glm/glm.hpp"


//
//	OtMap
//

class OtMap {
public:
	// biome types
	enum class Biome {
		none,
		snow,
		tundra,
		mountain,
		taiga,
		shrubland,
		temperateDesert,
		temperateRainForest,
		temperateDeciduousForest,
		grassland,
		tropicalRainForest,
		tropicalSeasonalForest,
		subtropicalDesert,
		ocean,
		lake,
		beach,

	};

	// clear the map
	void clear();

	// see if map is valid
	inline bool isValid() { return map != nullptr; }

	// update map
	void update(int seed, int size, float disturbance);

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
		Region(float x, float y) : center(x, y) {}
		glm::vec2 center;
		float height = 0.0f;
		float moisture = 0.0f;
		Biome biome = Biome::none;
		std::vector<size_t> polygon;
	};

	class Corner {
	public:
		Corner(glm::vec2 p) : position(p) {}
		glm::vec2 position;
		float height = 0.0f;
		float moisture = 0.0f;
		bool border = false;
		bool water = false;
		bool ocean = false;
		bool coast = false;
	};

	class Map {
	public:
		int seed;
		int size;
		float disturbance;
		std::vector<Region> regions;
		std::vector<Corner> corners;
		std::vector<size_t> triangles;
		std::vector<size_t> halfedges;
	};

	// properties
	std::shared_ptr<Map> map;
	int version = 0;

	// private functions to generate map
	void generateRegions();
	void triangulate();
	void generateCorners();

	bool isIsland(glm::vec2 pos);
};
