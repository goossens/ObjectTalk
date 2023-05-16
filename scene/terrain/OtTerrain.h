//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <filesystem>
#include <vector>

#include "glm/glm.hpp"
#include "nlohmann/json_fwd.hpp"

#include "OtTerrainMesh.h"
#include "OtTerrainTile.h"


//
//	OtTerrain
//

class OtTerrain {
public:
	// constructor
	OtTerrain();

	// GUI to change properties
	bool renderGUI();

	// (de)serialize
	nlohmann::json serialize(std::filesystem::path* basedir);
	void deserialize(nlohmann::json data, std::filesystem::path* basedir);

	// update the mesh tiles based on camera position
	void update(float hscale, float vscale, const glm::vec3& cameraPosition);

	// access properties
	bool isWireFrame() { return wireframe; }
	inline std::vector<OtTerrainMesh>& getMeshes() { return meshes; }

private:
	// scaling parameters
	float hScale = 1.0f;
	float vScale = 1.0f;

	// rendering type
	bool wireframe = false;

	// current terrain offsets
	float xoffset;
	float zoffset;

	// terrain tiles
	std::vector<OtTerrainTile> tiles;

	// terrain meshes
	std::vector<OtTerrainMesh> meshes;
};
