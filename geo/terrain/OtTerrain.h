//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>
#include <vector>

#include "nlohmann/json_fwd.hpp"

#include "OtCamera.h"
#include "OtIndexBuffer.h"
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
	void deserialize(nlohmann::json& data, std::string* basedir);

	// update state based on provided camera
	void update(OtCamera& camera);

	// access the meshes
	inline std::vector<OtTerrainMesh>& getMeshes() { return meshes; }

	// access properties
	inline void setTileSize(int value) { tileSize = value; }
	inline void setLOD(int value) { lod = value; }
	inline void setHorizontalScale(float value) { hScale = value; }
	inline void setVerticalScale(float value) { vScale = value; }
	inline void setVerticalOffset(float value) { vOffset = value; }
	inline void setCastShadow(bool value) { castShadow = value; }
	inline void setWireframe(bool value) { wireframe = value; }

	inline int getTileSize() { return tileSize; }
	inline int getLOD() { return lod; }
	inline float getHorizontalScale() { return hScale; }
	inline float getVerticalScale() { return vScale; }
	inline float getVerticalOffset() { return vOffset; }
	inline bool getCastShadow() { return castShadow; }
	inline bool getWireframe() { return wireframe; }

	inline OtTerrainHeights& getHeights() { return heights; }
	inline OtTerrainMaterial& getMaterial() { return material; }

private:
	// the scene renderer needs access to our properties
	friend class OtSceneRenderEntitiesPass;

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

	// current geoClipMap center
	float centerX;
	float centerZ;

	// create terrain entities
	static constexpr int noDegenerate = 0;
	static constexpr int topDegenerate = 1;
	static constexpr int leftDegenerate = 2;
	static constexpr int bottomDegenerate = 4;
	static constexpr int rightDegenerate = 8;

	// support functions
	void initialize();
	void clear();
	void createVertices();
	void createIndices(OtIndexBuffer& triangleBuffer, OtIndexBuffer& lineBuffer, int degenerate);
	void createTiles();
};
