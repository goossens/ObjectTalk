//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <set>
#include <thread>
#include <unordered_map>

#include "bgfx/bgfx.h"

#include "OtConcurrentQueue.h"
#include "OtHash.h"

#include "OtController.h"
#include "OtMaterial.h"
#include "OtSceneObject.h"
#include "OtTerrainMap.h"
#include "OtTerrainTile.h"


//
//	OtTerrainClass
//

class OtTerrainClass;
typedef std::shared_ptr<OtTerrainClass> OtTerrain;

class OtTerrainClass : public OtSceneObjectClass {
public:
	// constructor/destructor
	OtTerrainClass();
	~OtTerrainClass();

	// initialize
	OtObject init(size_t count, OtObject* parameters);

	// specify a terrainmap
	OtObject setTerrainMap(OtObject terrainmap);

	// set maximum viewing distance
	OtObject setViewingDistance(float distance);

	// specify region parameters
	OtObject setRegionTransitions(
		float transion1, float overlap1,
		float transion2, float overlap2,
		float transion3, float overlap3);

	OtObject setRegion1Color(float r, float g, float b) { region1Color = glm::vec3(r, g, b); return shared(); }
	OtObject setRegion2Color(float r, float g, float b) { region2Color = glm::vec3(r, g, b); return shared(); }
	OtObject setRegion3Color(float r, float g, float b) { region3Color = glm::vec3(r, g, b); return shared(); }
	OtObject setRegion4Color(float r, float g, float b) { region4Color = glm::vec3(r, g, b); return shared(); }

	OtObject setTextureScale(float scale);

	OtObject setRegion1Texture(const std::string& texture);
	OtObject setRegion2Texture(const std::string& texture);
	OtObject setRegion3Texture(const std::string& texture);
	OtObject setRegion4Texture(const std::string& texture);

	// update state
	void update(OtRenderingContext context) override;

	// render in BGFX
	void render(OtRenderingContext context) override;

	// GUI to change properties
	void renderGUI();

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtTerrain create();

protected:
	// mark changes and trigger tile regeneration
	void parametersHaveChanged();

	// material
	OtMaterial material;

	// region information
	float region1Transition = 1.0;
	float region2Transition = 15.0;
	float region3Transition = 25.0;

	float region1Overlap = 1.0;
	float region2Overlap = 5.0;
	float region3Overlap = 5.0;

	glm::vec3 region1Color = glm::vec3(0.965, 0.894, 0.678);
	glm::vec3 region2Color = glm::vec3(0.494, 0.784, 0.314);
	glm::vec3 region3Color = glm::vec3(0.584, 0.553, 0.522);
	glm::vec3 region4Color = glm::vec3(0.90, 0.90, 0.90);

	float textureScale = 50.0;

	bgfx::TextureHandle textureRegion1 = BGFX_INVALID_HANDLE;
	bgfx::TextureHandle textureRegion2 = BGFX_INVALID_HANDLE;
	bgfx::TextureHandle textureRegion3 = BGFX_INVALID_HANDLE;
	bgfx::TextureHandle textureRegion4 = BGFX_INVALID_HANDLE;

	// uniforms
	bgfx::UniformHandle terrainUniform = BGFX_INVALID_HANDLE;
	bgfx::UniformHandle textureUniform1 = BGFX_INVALID_HANDLE;
	bgfx::UniformHandle textureUniform2 = BGFX_INVALID_HANDLE;
	bgfx::UniformHandle textureUniform3 = BGFX_INVALID_HANDLE;
	bgfx::UniformHandle textureUniform4 = BGFX_INVALID_HANDLE;

	// BGFX embeddedShaders
	bgfx::ProgramHandle shader = BGFX_INVALID_HANDLE;
	bgfx::ProgramHandle shadowShader = BGFX_INVALID_HANDLE;

	// terrainmap
	OtTerrainMap terrainmap;
	size_t terrainmapID;

	// maximum viewing distance
	float maxViewingDist = 2000.0;

	// current version number
	size_t version = 1;

	// terrain tiles in use and/or visible
	std::unordered_map<size_t, OtTerrainTile> usedTiles;
	std::vector<OtTerrainTile> visibleTiles;


	bool tileExists(int x, int y, int lod, size_t version) {
		size_t hash = 0;
		OtHash(hash, x, y, lod, version);
		return usedTiles.count(hash) != 0;
	}

	bool tileRequested(int x, int y, int lod, size_t version) {
		size_t hash = 0;
		OtHash(hash, x, y, lod, version);
		return requested.count(hash) != 0;
	}

	OtTerrainTile tileGet(int x, int y, int lod, size_t version) {
		size_t hash = 0;
		OtHash(hash, x, y, lod, version);
		return usedTiles[hash];
	}

	// worker thread to generate tiles
	std::thread worker;
	std::set<size_t> requested;
	OtConcurrentQueue<OtTerrainTile> requests;
	OtConcurrentQueue<OtTerrainTile> responses;
	void generateTiles();
};


//
//	Controller widget
//

OT_CONTROLLER(Terrain)
