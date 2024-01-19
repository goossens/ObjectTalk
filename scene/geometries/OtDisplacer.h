//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <cstdint>
#include <string>
#include <vector>

#include "nlohmann/json_fwd.hpp"

#include "OtAABB.h"
#include "OtHeightMap.h"
#include "OtVertex.h"


//
//	OtDisplacer
//

class OtDisplacer {
public:
	// displace vertices
	void displace(std::vector<OtVertex>& vertices, std::vector<uint32_t>& triangles);

	// get the displaced vertices and bounding box
	std::vector<OtVertex>& getVertices() { return displaced; }
	OtAABB& getAABB() { return aabb; }

	// see if displacement is active
	bool isActive() { return (state == Noise) || (state == Map && heightmap.isValid()); }

	// UI to change properties
	bool renderUI();

	// (de)serialize properties
	nlohmann::json serialize(std::string* basedir);
	void deserialize(nlohmann::json data, std::string* basedir);

private:
	// displacement functions
	void updateHeightmap();
	void displaceByMap(std::vector<OtVertex>& vertices, std::vector<uint32_t>& triangles);
	void displaceByNoise(std::vector<OtVertex>& vertices, std::vector<uint32_t>& triangles);
	void recalculateNormals(std::vector<uint32_t>& triangles);

	// state
	enum {
		Off,
		Map,
		Noise
	};

	int state = Off;

	// displacement map properies
	std::string map;
	OtHeightMap heightmap;

	// noise displacement properties
	int seed = 1337;
	int octaves = 3;
	float frequency = 0.01f;
	float lacunarity = 1.0f;
	float persistance = 0.5f;
	float scale = 0.5f;

	// displaced vertices
	std::vector<OtVertex> displaced;
	OtAABB aabb;
};
