//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <vector>

#include "bgfx/embedded_shader.h"
#include "imgui.h"

#include "OtFunction.h"

#include "OtAABB.h"
#include "OtFramework.h"
#include "OtTerrain.h"

#include "OtShadowShader.h"
#include "OtTerrainShader.h"


//
//	Constants
//

static const int tileSize = 242;


//
//	Globals
//

static const bgfx::EmbeddedShader embeddedShaders[] = {
	BGFX_EMBEDDED_SHADER(OtTerrainVS),
	BGFX_EMBEDDED_SHADER(OtTerrainFS),
	BGFX_EMBEDDED_SHADER(OtShadowVS),
	BGFX_EMBEDDED_SHADER(OtShadowFS),
	BGFX_EMBEDDED_SHADER_END()
};


//
//	OtTerrainClass::OtTerrainClass
//

OtTerrainClass::OtTerrainClass() {
	// create a worker thread to generate tiles
	worker = std::thread([this]() {
		this->generateTiles();
		return 0;
	});

	// register uniforms
	terrainUniform = bgfx::createUniform("u_terrain", bgfx::UniformType::Vec4, 7);
	textureUniform1 = bgfx::createUniform("s_texture_1", bgfx::UniformType::Sampler);
	textureUniform2 = bgfx::createUniform("s_texture_2", bgfx::UniformType::Sampler);
	textureUniform3 = bgfx::createUniform("s_texture_3", bgfx::UniformType::Sampler);
	textureUniform4 = bgfx::createUniform("s_texture_4", bgfx::UniformType::Sampler);

	// setup material
	material = OtMaterialClass::create();
	material->setColorVector(glm::vec3(1.0));
	material->setAmbientVector(glm::vec3(0.6, 0.6, 0.6));
	material->setDiffuseVector(glm::vec3(0.6, 0.6, 0.6));
	material->setSpecularVector(glm::vec3(0.5, 0.5, 0.5));
	material->setShininess(20.0);

	// initialize shaders
	bgfx::RendererType::Enum type = bgfx::getRendererType();

	shader = bgfx::createProgram(
		bgfx::createEmbeddedShader(embeddedShaders, type, "OtTerrainVS"),
		bgfx::createEmbeddedShader(embeddedShaders, type, "OtTerrainFS"),
		true);

	shadowShader = bgfx::createProgram(
		bgfx::createEmbeddedShader(embeddedShaders, type, "OtShadowVS"),
		bgfx::createEmbeddedShader(embeddedShaders, type, "OtShadowFS"),
		true);
}


//
//	OtTerrainClass::~OtTerrainClass
//

OtTerrainClass::~OtTerrainClass() {
	// stop worker thread
	requests.push(nullptr);
	worker.join();

	// detach from noisemap (if required)
	if (terrainmap) {
		terrainmap->detach(terrainmapID);
	}

	// cleanup
	bgfx::destroy(terrainUniform);
	bgfx::destroy(textureUniform1);
	bgfx::destroy(textureUniform2);
	bgfx::destroy(textureUniform3);
	bgfx::destroy(textureUniform4);

	bgfx::destroy(shader);
	bgfx::destroy(shadowShader);
}


//
//	OtTerrainClass::init
//

OtObject OtTerrainClass::init(size_t count, OtObject* parameters) {
	if (count == 1) {

	} else if (count != 0) {
		OtExcept("[Terrain] constructor expects 0 or 1 arguments (not %ld)", count);
	}

	return nullptr;
}


//
//	OtTerrainClass::setTerrainMap
//

OtObject OtTerrainClass::setTerrainMap(OtObject object) {
	// sanity check
	object->expectKindOf("TerrainMap");

	// cleanup
	if (terrainmap) {
		terrainmap->detach(terrainmapID);
		terrainmap = nullptr;
	}

	// set new terrainmap
	terrainmap = object->cast<OtTerrainMapClass>();
	parametersHaveChanged();

	terrainmapID = terrainmap->attach([this]() {
		parametersHaveChanged();
	});

	return shared();
}


//
//	OtTerrainClass::setViewingDistance
//

OtObject OtTerrainClass::setViewingDistance(float distance) {
	maxViewingDist = distance;
	return shared();
}


//
//	OtTerrainClass::setRegionTransitions
//

OtObject OtTerrainClass::setRegionTransitions(float transion1, float overlap1, float transion2, float overlap2, float transion3, float overlap3) {
	region1Transition = transion1;
	region2Transition = transion2;
	region3Transition = transion3;
	region1Overlap = overlap1;
	region2Overlap = overlap2;
	region3Overlap = overlap3;
	return shared();
}


//
//	OtTerrainClass::setTextureScale
//

OtObject OtTerrainClass::setTextureScale(float scale) {
	textureScale = scale;
	return shared();
}


//
//	OtTerrainClass::setRegion1Texture
//

OtObject OtTerrainClass::setRegion1Texture(const std::string& textureName) {
	if (bgfx::isValid(textureRegion1)) {
		OtExcept("Texture 1 already set for [Terrain] widget");
	}

	textureRegion1 = OtFrameworkClass::instance()->getTexture(textureName);
	return shared();
}


//
//	OtTerrainClass::setRegion2Texture
//

OtObject OtTerrainClass::setRegion2Texture(const std::string& textureName) {
	if (bgfx::isValid(textureRegion2)) {
		OtExcept("Texture 2 already set for [Terrain] widget");
	}

	textureRegion2 = OtFrameworkClass::instance()->getTexture(textureName);
	return shared();
}


//
//	OtTerrainClass::setRegion3Texture
//

OtObject OtTerrainClass::setRegion3Texture(const std::string& textureName) {
	if (bgfx::isValid(textureRegion3)) {
		OtExcept("Texture 3 already set for [Terrain] widget");
	}

	textureRegion3 = OtFrameworkClass::instance()->getTexture(textureName);
	return shared();
}


//
//	OtTerrainClass::setRegion4Texture
//

OtObject OtTerrainClass::setRegion4Texture(const std::string& textureName) {
	if (bgfx::isValid(textureRegion4)) {
		OtExcept("Texture 4 already set for [Terrain] widget");
	}

	textureRegion4 = OtFrameworkClass::instance()->getTexture(textureName);
	return shared();
}


//
//	OtTerrainClass::update
//

void OtTerrainClass::update(OtRenderingContext context) {
	// sanity check
	if (!terrainmap) {
		OtExcept("TerrainMap] properties not set for [Terrain]");
	}

	// remove old tiles (use std::remove_if in C++20)
	auto now = OtFrameworkClass::instance()->getTime();
	auto entry = usedTiles.begin();

	while (entry != usedTiles.end()) {
		auto const& [hash, tile] = *entry;

		if (now - tile->lastUsed > 5.0) {
			entry = usedTiles.erase(entry);

		} else {
			entry++;
		}
	}

	// process worker outputs
	if (!responses.empty()) {
		OtTerrainTile tile = responses.pop();

		if (tile->version == version) {
			usedTiles[tile->hash] = tile;
			requested.erase(tile->hash);
		}
	}

	// list of tiles needed
	struct neededTile {
		neededTile(int _x, int _y, int l, size_t v, float d) : x(_x), y(_y), lod(l), version(v), distance(d) {}
		int x;
		int y;
		int lod;
		size_t version;
		float distance;
	};

	std::vector<neededTile> needed;

	// clear list of visible tiles
	visibleTiles.clear();

	// determine number of tiles visible from center
	auto visibleFromCenter = (int) std::lround(maxViewingDist / tileSize);

	// determine center tile
	glm::vec3 center = context->getCamera()->getPosition();
	auto terrainCenterTileX = (int) std::lround(center.x / tileSize);
	auto terrainCenterTileY = (int) std::lround(center.z / tileSize);

	// get vertical limits
	auto minHeight = terrainmap->getMinHeight();
	auto maxHeight = terrainmap->getMaxHeight();

	// process all possible visible tiles
	for (auto yOffset = -visibleFromCenter; yOffset <= visibleFromCenter; yOffset++) {
		for (auto xOffset = -visibleFromCenter; xOffset <= visibleFromCenter; xOffset++) {
			auto x = terrainCenterTileX + xOffset;
			auto y = terrainCenterTileY + yOffset;

			// ensure tile is visible in camera frustum
			auto cx = x * tileSize;
			auto cy = y * tileSize;

			auto camera = context->getCamera();

			auto aabb = OtAABBClass::create();
			aabb->addPoint(glm::vec3(cx - tileSize / 2, minHeight, cy - tileSize / 2));
			aabb->addPoint(glm::vec3(cx + tileSize / 2, maxHeight, cy + tileSize / 2));

			if (camera->isVisibleAABB(aabb)) {
				// determine level of detail for tile
				float distanceToTile = glm::distance(glm::vec3(cx, 0.0, cy), camera->getPosition());
				int distanceInTiles = distanceToTile / tileSize;
				int lod = distanceInTiles <= 6 ? 6 - distanceInTiles : 0;

				// see if tile already exists?
				if (tileExists(x, y, lod, version)) {
					// yes, add it to list of visible tiles
					OtTerrainTile tile = tileGet(x, y, lod, version);
					tile->lastUsed = now;
					visibleTiles.push_back(tile);

				} else {
					// no, mark tile as needed if not already requested
					if (!tileRequested(x, y, lod, version)) {
						// create a new tile and add it to the request list
						needed.push_back(neededTile(x, y, lod, version, distanceToTile));
					}

					// see if we have a substitude for now
					if (tileExists(x, y, lod, version - 1)) {
						OtTerrainTile tile = tileGet(x, y, lod, version - 1);
						tile->lastUsed = now;
						visibleTiles.push_back(tile);

					} else {
						OtTerrainTile tile = nullptr;
						int lodDistance = 7;

						for (auto l = 0; l <= 6; l++) {
							if (tileExists(x, y, l, version)) {
								if (std::abs(lod - l) < lodDistance) {
									lodDistance = std::abs(lod - l);
									tile = tileGet(x, y, l, version);
								}
							}

							if (tile) {
								tile->lastUsed = now;
								visibleTiles.push_back(tile);
							}
						}
					}
				}
			}
		}
	}

	// sort list of needed tiles by distance
	std::sort(needed.begin(), needed.end(), [](neededTile const& a, neededTile const& b) {
		return a.distance < b.distance;
	});

	// request all needed tiles (closest ones first)
	for (auto& n : needed) {
		OtTerrainTile tile = OtTerrainTileClass::create(n.x, n.y, n.lod, n.version);
		requested.insert(tile->hash);
		requests.push(tile);
	}
}


//
//	OtTerrainClass::render
//

void OtTerrainClass::render(OtRenderingContext context) {
	// don't render if this is a shadowmap and we cast no shadow
	if (!context->inShadowmapPhase() || castShadowFlag) {
		// render all visible tiles
		for (auto& tile : visibleTiles) {
			// set context uniforms
			context->submit(receivesShadow());

			// set material uniforms
			material->submit();

			// set land uniforms
			glm::vec4 terrainUniforms[7];
			terrainUniforms[0].x = textureScale;
			terrainUniforms[0].y = context->inReflectionPhase();
			terrainUniforms[0].z = context->inRefractionPhase();

			terrainUniforms[1].x = region1Transition;
			terrainUniforms[1].y = region2Transition;
			terrainUniforms[1].z = region3Transition;

			terrainUniforms[2].x = region1Overlap;
			terrainUniforms[2].y = region2Overlap;
			terrainUniforms[2].z = region3Overlap;

			terrainUniforms[3] = glm::vec4(region1Color, bgfx::isValid(textureRegion1));
			terrainUniforms[4] = glm::vec4(region2Color, bgfx::isValid(textureRegion2));
			terrainUniforms[5] = glm::vec4(region3Color, bgfx::isValid(textureRegion1));
			terrainUniforms[6] = glm::vec4(region4Color, bgfx::isValid(textureRegion2));

			bgfx::setUniform(terrainUniform, terrainUniforms, 7);

			// set textures
			bgfx::TextureHandle dummy = OtFrameworkClass::instance()->getDummyTexture();
			bgfx::setTexture(1, textureUniform1, bgfx::isValid(textureRegion1) ? textureRegion1 : dummy);
			bgfx::setTexture(2, textureUniform2, bgfx::isValid(textureRegion2) ? textureRegion2 : dummy);
			bgfx::setTexture(3, textureUniform3, bgfx::isValid(textureRegion3) ? textureRegion3 : dummy);
			bgfx::setTexture(4, textureUniform4, bgfx::isValid(textureRegion4) ? textureRegion4 : dummy);

			// submit vertices and triangles
			tile->submit();

			// run shader
			if (context->inShadowmapPhase()) {
				// we use a cheap shader if we are creating a shadowmap
				bgfx::setState(BGFX_STATE_WRITE_Z | BGFX_STATE_DEPTH_TEST_LESS | BGFX_STATE_CULL_CCW | BGFX_STATE_MSAA);
				bgfx::submit(context->getView(), shadowShader);

			} else {
				// we go all out with our terrain shader
				bgfx::setState(BGFX_STATE_DEFAULT);
				bgfx::submit(context->getView(), shader);
			}
		}
	}
}


//
//	OtTerrainClass::renderGUI
//

void OtTerrainClass::renderGUI() {
	ImGui::Checkbox("Enabled", &enabled);
	ImGui::Checkbox("Casts Shadow", &castShadowFlag);
	ImGui::Checkbox("Receives Shadow", &receiveShadowFlag);

	ImGui::SliderFloat("Viewing Distance", &maxViewingDist, 200, 10000);
	ImGui::SliderFloat("Texture Scale", &textureScale, 1, 400);

	ImGui::SliderFloat("Region 1", &region1Transition, 0, 300);
	ImGui::SliderFloat("Overlap 1", &region1Overlap, 1, 50);
	ImGui::SliderFloat("Region 2", &region2Transition, 0, 300);
	ImGui::SliderFloat("Overlap 2", &region2Overlap, 1, 50);
	ImGui::SliderFloat("Region 3", &region3Transition, 0, 300);
	ImGui::SliderFloat("Overlap 3", &region3Overlap, 1, 50);

	if (ImGui::TreeNodeEx("Debug", ImGuiTreeNodeFlags_Framed)) {
		auto tilesInUse = (int) usedTiles.size();
		int count[7] = {0};

		for (auto& entry : usedTiles) {
			count[entry.second->lod]++;
		}

		ImGui::Text("Tiles in use: %d", tilesInUse);
		ImGui::Text("Tiles LOD 0: %d", count[0]);
		ImGui::Text("Tiles LOD 1: %d", count[1]);
		ImGui::Text("Tiles LOD 2: %d", count[2]);
		ImGui::Text("Tiles LOD 3: %d", count[3]);
		ImGui::Text("Tiles LOD 4: %d", count[4]);
		ImGui::Text("Tiles LOD 5: %d", count[5]);
		ImGui::Text("Tiles LOD 6: %d", count[6]);

		ImGui::TreePop();
	}
}


//
//	OtTerrainClass::generateTiles
//

void OtTerrainClass::generateTiles() {
	// keep this thread running until we get told to stop
	bool done = false;

	while (!done) {
		// get next tile request
		OtTerrainTile tile = requests.pop();

		// a nullptr indicates that we are done
		if (!tile) {
			done = true;

		} else {
			// generate tile
			tile->generate(terrainmap);

			// return result
			responses.push(tile);
		}
	}
}


//
//	OtTerrainClass::parametersHaveChanged
//

void OtTerrainClass::parametersHaveChanged() {
	// clear request queue
	requests.clear();
	requested.clear();
	version++;
}


//
//	OtTerrainClass::getMeta
//

OtType OtTerrainClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtTerrainClass>("Terrain", OtSceneObjectClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtTerrainClass::init));

		type->set("setTerrainMap", OtFunctionClass::create(&OtTerrainClass::setTerrainMap));
		type->set("setViewingDistance", OtFunctionClass::create(&OtTerrainClass::setViewingDistance));

		type->set("setRegionTransitions", OtFunctionClass::create(&OtTerrainClass::setRegionTransitions));

		type->set("setRegion1Color", OtFunctionClass::create(&OtTerrainClass::setRegion1Color));
		type->set("setRegion2Color", OtFunctionClass::create(&OtTerrainClass::setRegion2Color));
		type->set("setRegion3Color", OtFunctionClass::create(&OtTerrainClass::setRegion3Color));
		type->set("setRegion4Color", OtFunctionClass::create(&OtTerrainClass::setRegion4Color));

		type->set("setTextureScale", OtFunctionClass::create(&OtTerrainClass::setTextureScale));

		type->set("setRegion1Texture", OtFunctionClass::create(&OtTerrainClass::setRegion1Texture));
		type->set("setRegion2Texture", OtFunctionClass::create(&OtTerrainClass::setRegion2Texture));
		type->set("setRegion3Texture", OtFunctionClass::create(&OtTerrainClass::setRegion3Texture));
		type->set("setRegion4Texture", OtFunctionClass::create(&OtTerrainClass::setRegion4Texture));
	}

	return type;
}


//
//	OtTerrainClass::create
//

OtTerrain OtTerrainClass::create() {
	OtTerrain terrain = std::make_shared<OtTerrainClass>();
	terrain->setType(getMeta());
	return terrain;
}
