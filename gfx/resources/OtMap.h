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

#include "OtFrameBuffer.h"


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

	// render to framebuffer
	void render(OtFrameBuffer& framebuffer);

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
		Region(float x, float y, bool b=false) : center(x, y), border(b) {}
		glm::vec2 center;
		float height = 0.0f;
		float moisture = 0.0f;
		Biome biome = Biome::none;
		bool border;
		bool water = false;
		bool ocean = false;
		bool coast = false;
		std::vector<size_t> corners;
	};

	class Corner {
	public:
		Corner(glm::vec2 p, bool b=false) : position(p), border(b) {}
		glm::vec2 position;
		float height = 0.0f;
		float moisture = 0.0f;
		bool border;
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
	void assignWater();
	void assignOceans();

	// utility functions
	inline size_t triangleOfEdge(size_t e) { return e / 3; }
	inline size_t nextHalfedge(size_t e) { return (e % 3 == 2) ? e - 2 : e + 1; }
	void getPolygon(size_t start, std::vector<glm::vec2>& p);
};
