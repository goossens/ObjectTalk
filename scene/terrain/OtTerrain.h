//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>
#include <vector>

#include "glm/glm.hpp"
#include "nlohmann/json_fwd.hpp"

#include "OtAABB.h"
#include "OtCamera.h"
#include "OtIndexBuffer.h"
#include "OtNormalMapper.h"
#include "OtTileableFbm.h"
#include "OtVertexBuffer.h"

#include "OtTerrainHeights.h"
#include "OtTerrainMaterial.h"
#include "OtTerrainMesh.h"
#include "OtTerrainTile.h"


//
//	OtTerrain
//

class OtTerrain {
public:
	// UI to change properties
	bool renderUI();

	// (de)serialize properties
	nlohmann::json serialize(std::string* basedir);
	void deserialize(nlohmann::json data, std::string* basedir);

	// access the members
	OtVertexBuffer& getVertices();
	std::vector<OtTerrainMesh>& getMeshes(OtCamera& camera);

	// are we rendering a wireframe
	inline bool isWireframe() { return wireframe; }

	// is the terrain casting a shadow
	inline bool isCastingShadow() { return castShadow; }

private:
	friend class OtSceneRendererContext;

	// terrain properties
	int tileSize = 32;
	int lod = 4;
	float hScale = 1.0f;
	float vScale = 1.0f;
	float vOffset = 0.5f;
	bool castShadow = true;
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

	// terrain heights and material
	OtTerrainHeights heights;
	OtTerrainMaterial material;
	OtTileableFbm tileableFbm;
	OtNormalMapper normalMapper;

	// current geoClipMap center
	float centerX;
	float centerZ;

	// create terrain entities
	static inline constexpr int noDegenerate = 0;
	static inline constexpr int topDegenerate = 1;
	static inline constexpr int leftDegenerate = 2;
	static inline constexpr int bottomDegenerate = 4;
	static inline constexpr int rightDegenerate = 8;

	void initialize();
	void clear();
	void createVertices();
	void createIndices(OtIndexBuffer& triangleBuffer, OtIndexBuffer& lineBuffer, int degenerate);
	void createTiles();
};
