//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <vector>

#include "glm/glm.hpp"
#include "nlohmann/json_fwd.hpp"

#include "OtFrustum.h"


//
//	OtTerrainComponent
//

class OtTerrainComponent {
public:
	// constructor
	OtTerrainComponent();

	// GUI to change component properties
	bool renderGUI();

	// (de)serialize component
	nlohmann::json serialize(std::filesystem::path* basedir);
	void deserialize(nlohmann::json data, std::filesystem::path* basedir);

	// get list of visible terrain tiles
	void getTiles(glm::vec3 center, OtFrustum& frustum);

	// component name
	static constexpr char const* name = "Terrain";

	// stored properties
	static constexpr int OtTerrainTileSize = 256;
	float maxViewingDist = 2000.0f;
	float hScale = 1.0f;
	float vScale = 1.0f;
	float vOffset = 0.5f;
	bool wireframe = false;

	// runtime properties
	struct Tile {
		int x;
		int y;
		int lod;
	};

	std::vector<Tile> tiles;
};
