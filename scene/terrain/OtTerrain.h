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

#include "OtFrustum.h"
#include "OtIndexBuffer.h"
#include "OtVertexBuffer.h"

#include "OtTerrainHeight.h"
#include "OtTerrainMesh.h"
#include "OtTerrainTile.h"


//
//	OtTerrain
//

class OtTerrain {
public:
	// GUI to change properties
	bool renderGUI();

	// (de)serialize properties
	nlohmann::json serialize(std::filesystem::path* basedir);
	void deserialize(nlohmann::json data, std::filesystem::path* basedir);

	// access the meshes
	std::vector<OtTerrainMesh>& getMeshes(OtFrustum& frustum, const glm::vec3& camera);

	// get the heightmap size
	int getHeightmapSize() { return heightmap.getSize(); }

	// bind heightmap to specified sampler
	void bindHeightmap(OtSampler& sampler, int unit) { heightmap.bindHeightmap(sampler, unit); }

	// are we rendering a wireframe
	inline bool isWireframe() { return wireframe; }

private:
	// terrain properties
	int tileSize = 32;
	int lods = 4;
	float hScale = 1.0f;
	float vScale = 1.0f;
	float vOffset = 0.5f;
	bool wireframe = false;

	// terrain geometry
	OtVertexBuffer vertices;
	OtIndexBuffer fullTriangles;
	OtIndexBuffer sideTriangles;
	OtIndexBuffer cornerTriangles;
	OtIndexBuffer fullLines;
	OtIndexBuffer sideLines;
	OtIndexBuffer cornerLines;

	std::vector<OtTerrainTile> centerTiles;
	std::vector<OtTerrainTile> ringTiles;
	std::vector<OtTerrainMesh> meshes;

	OtTerrainHeight heightmap;

	// current geoclipmap center
	float centerX;
	float centerZ;

	// create terrain entities
	enum {
		noDegenerate = 0,
		topDegenerate = 1,
		leftDegenerate = 2,
		bottomDegenerate = 4,
		rightDegenerate = 8
	};

	void clear();
	void createVertices();
	void createIndices(OtIndexBuffer& triangleBuffer, OtIndexBuffer& lineBuffer, int degenerate);
	void createTiles();
};
