//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <vector>

#include "ot/function.h"

#include "application.h"
#include "terrain.h"
#include "terrainshader.h"


//
//	Constants
//

static const int tileSize = 242;


//
//	Globals
//

static const bgfx::EmbeddedShader embeddedShaders[] = {
	BGFX_EMBEDDED_SHADER(vs_terrain),
	BGFX_EMBEDDED_SHADER(fs_terrain),
	BGFX_EMBEDDED_SHADER_END()
};


//
//	OtTerrainClass::OtTerrainClass
//

OtTerrainClass::OtTerrainClass() {
	// create a worker thread to generate tiles
	worker = std::thread([this]() {
		this->generateTiles(this);
		return 0;
	});

	// register uniforms
	materialUniform = bgfx::createUniform("u_material", bgfx::UniformType::Vec4, 5);
	terrainUniform = bgfx::createUniform("u_terrain", bgfx::UniformType::Vec4, 7);
	textureUniform1 = bgfx::createUniform("s_texture_1", bgfx::UniformType::Sampler);
	textureUniform2 = bgfx::createUniform("s_texture_2", bgfx::UniformType::Sampler);
	textureUniform3 = bgfx::createUniform("s_texture_3", bgfx::UniformType::Sampler);
	textureUniform4 = bgfx::createUniform("s_texture_4", bgfx::UniformType::Sampler);

	// initialize shader
	bgfx::RendererType::Enum type = bgfx::getRendererType();

	shader = bgfx::createProgram(
		bgfx::createEmbeddedShader(embeddedShaders, type, "vs_terrain"),
		bgfx::createEmbeddedShader(embeddedShaders, type, "fs_terrain"),
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
	if (noisemap) {
		noisemap->detach(noisemapID);
	}

	// cleanup
	bgfx::destroy(materialUniform);
	bgfx::destroy(terrainUniform);

	bgfx::destroy(textureUniform1);
	bgfx::destroy(textureUniform2);
	bgfx::destroy(textureUniform3);
	bgfx::destroy(textureUniform4);

	bgfx::destroy(shader);
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
//	OtTerrainClass::setNoiseMap
//

OtObject OtTerrainClass::setNoiseMap(OtObject object) {
	// sanity check
	if (!object->isKindOf("NoiseMap")) {
		OtExcept("Expected a [NoiseMap] object, not a [%s]", object->getType()->getName().c_str());
	}

	// cleanup
	if (noisemap) {
		noisemap->detach(noisemapID);
		noisemap = nullptr;
	}

	// set new noisemap
	noisemap = object->cast<OtNoiseMapClass>();
	parametersHaveChanged();

	noisemapID = noisemap->attach([this]() {
		parametersHaveChanged();
	});

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

OtObject OtTerrainClass::setRegion1Texture(OtObject object) {
	// ensure object is a valid texture
	if (!object) {
		textureRegion1 = nullptr;

	} else if (object->isKindOf("Texture")) {
		textureRegion1 = object->cast<OtTextureClass>();

	} else {
		OtExcept("Expected a [Texture] object, not a [%s]", object->getType()->getName().c_str());
	}

	return shared();
}


//
//	OtTerrainClass::setRegion2Texture
//

OtObject OtTerrainClass::setRegion2Texture(OtObject object) {
	// ensure object is a valid texture
	if (!object) {
		textureRegion2 = nullptr;

	} else if (object->isKindOf("Texture")) {
		textureRegion2 = object->cast<OtTextureClass>();

	} else {
		OtExcept("Expected a [Texture] object, not a [%s]", object->getType()->getName().c_str());
	}

	return shared();
}


//
//	OtTerrainClass::setRegion3Texture
//

OtObject OtTerrainClass::setRegion3Texture(OtObject object) {
	// ensure object is a valid texture
	if (!object) {
		textureRegion3 = nullptr;

	} else if (object->isKindOf("Texture")) {
		textureRegion3 = object->cast<OtTextureClass>();

	} else {
		OtExcept("Expected a [Texture] object, not a [%s]", object->getType()->getName().c_str());
	}

	return shared();
}


//
//	OtTerrainClass::setRegion4Texture
//

OtObject OtTerrainClass::setRegion4Texture(OtObject object) {
	// ensure object is a texture
	if (!object) {
		textureRegion4 = nullptr;

	} else if (object->isKindOf("Texture")) {
		textureRegion4 = object->cast<OtTextureClass>();

	} else {
		OtExcept("Expected a [Texture] object, not a [%s]", object->getType()->getName().c_str());
	}

	return shared();
}


//
//	OtTerrainClass::render
//

void OtTerrainClass::render(OtRenderingContext* context) {
	// sanity check
	if (!noisemap) {
		OtExcept("[NoiseMap] properties not set for [Terrain]");
	}

	// remove old tiles (use std::remove_if in C++20)
	auto now = OtApplicationClass::getTime();
	auto entry = tiles.begin();

	while (entry != tiles.end()) {
		auto const& [hash, tile] = *entry;

		if (now - tile->lastUsed > 5.0) {
			entry = tiles.erase(entry);

		} else {
			entry++;
		}
	}

	// process worker outputs
	if (!responses.empty()) {
		std::shared_ptr<OtTerrainTile> tile = responses.pop();

		if (tile->version == version) {
			tiles[tile->hash] = tile;
			requested.erase(tile->hash);
		}
	}

	// list of tiles that are visible (and available)
	std::vector<std::shared_ptr<OtTerrainTile>> visible;

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

	// determine number of tiles visible from center
	int visibleTiles = std::lround(maxViewingDist / tileSize);

	// determine center tile
	int terrainCenterTileX = std::lround(center.x / tileSize);
	int terrainCenterTileY = std::lround(center.y / tileSize);

	// get vertical limits
	auto minNoise = noisemap->getMinNoise();
	auto maxNoise = noisemap->getMaxNoise();

	// process all possible visible tiles
	for (auto yOffset = -visibleTiles; yOffset <= visibleTiles; yOffset++) {
		for (auto xOffset = -visibleTiles; xOffset <= visibleTiles; xOffset++) {
			auto x = terrainCenterTileX + xOffset;
			auto y = terrainCenterTileY + yOffset;

			// ensure tile is visible in camera frustum
			auto cx = x * tileSize;
			auto cy = y * tileSize;

			glm::vec3 minValues = glm::vec3(cx - tileSize / 2, minNoise, cy - tileSize / 2);
			glm::vec3 maxValues = glm::vec3(cx + tileSize / 2, maxNoise, cy + tileSize / 2);

			if (context->camera->isVisibleAABB(minValues, maxValues)) {
				// determine level of detail for tile
				float distanceToTile = glm::distance(glm::vec3(cx, 0.0, cy), context->camera->getPosition());
				int distanceInTiles = std::lround(distanceToTile / tileSize);
				int lod = distanceInTiles <= 6 ? 6 - distanceInTiles : 0;

				// see if tile already exists?
				if (tileExists(x, y, lod, version)) {
					// yes, add it to list of visible tiles
					std::shared_ptr<OtTerrainTile> tile = tileGet(x, y, lod, version);
					tile->lastUsed = now;
					visible.push_back(tile);

				} else {
					// no, mark tile as needed if not already requested
					if (!tileRequested(x, y, lod, version)) {
						// create a new tile and add it to the request list
						needed.push_back(neededTile(x, y, lod, version, distanceToTile));
					}

					// see if we have a substitude for now
					if (tileExists(x, y, lod, version - 1)) {
						std::shared_ptr<OtTerrainTile> tile = tileGet(x, y, lod, version - 1);
						tile->lastUsed = now;
						visible.push_back(tile);

					} else if (tileExists(x, y, lod + 1, version)) {
						std::shared_ptr<OtTerrainTile> tile = tileGet(x, y, lod + 1, version);
						tile->lastUsed = now;
						visible.push_back(tile);

					} else if (tileExists(x, y, lod - 1, version)) {
						std::shared_ptr<OtTerrainTile> tile = tileGet(x, y, lod - 1, version);
						tile->lastUsed = now;
						visible.push_back(tile);
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
		std::shared_ptr<OtTerrainTile> tile = std::make_shared<OtTerrainTile>(
			n.x,
			n.y,
			n.lod,
			n.version);

		requested.insert(tile->hash);
		requests.push(tile);
	}

	// render all tiles
	for (auto& tile : visible) {
		// set context uniforms
		context->submit();

		// set material uniforms
		glm::vec4 materialUniforms[6];
		materialUniforms[0] = glm::vec4(1.0);
		materialUniforms[1] = glm::vec4(0.6, 0.6, 0.6, 1.0);
		materialUniforms[2] = glm::vec4(0.6, 0.6, 0.6, 1.0);
		materialUniforms[3] = glm::vec4(0.5, 0.5, 0.5, 1.0);
		materialUniforms[4].x = 20.0;
		bgfx::setUniform(materialUniform, &materialUniforms, 5);

		// set land uniforms
		glm::vec4 terrainUniforms[7];
		terrainUniforms[0].x = textureScale;
		terrainUniforms[0].y = context->reflection;
		terrainUniforms[0].z = context->refraction;

		terrainUniforms[1].x = region1Transition;
		terrainUniforms[1].y = region2Transition;
		terrainUniforms[1].z = region3Transition;

		terrainUniforms[2].x = region1Overlap;
		terrainUniforms[2].y = region2Overlap;
		terrainUniforms[2].z = region3Overlap;

		terrainUniforms[3] = glm::vec4(region1Color, textureRegion1 != nullptr);
		terrainUniforms[4] = glm::vec4(region2Color, textureRegion2 != nullptr);
		terrainUniforms[5] = glm::vec4(region3Color, textureRegion3 != nullptr);
		terrainUniforms[6] = glm::vec4(region4Color, textureRegion4 != nullptr);

		bgfx::setUniform(terrainUniform, terrainUniforms, 7);

		// set textures
		(textureRegion1 ? textureRegion1 : OtTextureClass::dummy())->submit(0, textureUniform1);
		(textureRegion2 ? textureRegion2 : OtTextureClass::dummy())->submit(1, textureUniform2);
		(textureRegion3 ? textureRegion3 : OtTextureClass::dummy())->submit(2, textureUniform3);
		(textureRegion4 ? textureRegion4 : OtTextureClass::dummy())->submit(3, textureUniform4);

		// render tileExists	// submit vertices and triangles
		bgfx::setVertexBuffer(0, tile->vertexBuffer);
		bgfx::setIndexBuffer(tile->indexBuffer);

		// run shader
		bgfx::setState(BGFX_STATE_DEFAULT);
		bgfx::submit(context->view, shader);
	}
}


//
//	OtTerrainClass::renderGUI
//

void OtTerrainClass::renderGUI() {
	bool changed = false;

	ImGui::Checkbox("Enabled", &enabled);
	ImGui::SliderFloat("Texture Scale", &textureScale, 0, 100);

	ImGui::SliderFloat("Region 1", &region1Transition, 0, 300);
	ImGui::SliderFloat("Overlap 1", &region1Overlap, 1, 50);
	ImGui::SliderFloat("Region 2", &region2Transition, 0, 300);
	ImGui::SliderFloat("Overlap 2", &region2Overlap, 1, 50);
	ImGui::SliderFloat("Region 3", &region3Transition, 0, 300);
	ImGui::SliderFloat("Overlap 3", &region3Overlap, 1, 50);
}


//
//	OtTerrainClass::generateTiles
//

void OtTerrainClass::generateTiles(OtTerrainClass* terrain) {
	// keep this thread running until we get told to stop
	bool done = false;

	while (!done) {
		// get next tile request
		std::shared_ptr<OtTerrainTile> tile = requests.pop();

		// a nullptr indicates that we are done
		if (!tile) {
			done = true;

		} else {
			// generate tile
			tile->generate(terrain->noisemap);

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
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtTerrainClass>("Terrain", OtSceneObjectClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtTerrainClass::init));

		type->set("setNoiseMap", OtFunctionClass::create(&OtTerrainClass::setNoiseMap));

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


//
//	OtTerrainTile::~OtTerrainTile
//

OtTerrainClass::OtTerrainTile::~OtTerrainTile() {
	// release resources
	if (bgfx::isValid(vertexBuffer)) {
		bgfx::destroy(vertexBuffer);
	}

	if (bgfx::isValid(indexBuffer)) {
		bgfx::destroy(indexBuffer);
	}
}

//
//	OtTerrainTile::generate
//

void OtTerrainClass::OtTerrainTile::generate(OtNoiseMap noisemap) {
	// dimensions
	auto tileOffsetX = x * tileSize - tileSize / 2;
	auto tileOffsetY = y * tileSize - tileSize / 2;

	auto lodSize = tileSize - 2;
	auto lodIncrement = lod >= 6 ? 1 : (6 - lod) * 2;
	auto lodSegments = lodSize / lodIncrement;
	auto lodVertices = lodSegments + 1;
	auto lodOffsetX = tileOffsetX + 1;
	auto lodOffsetY = tileOffsetY + 1;

	auto heightMapVertices = lodVertices + 2;
	auto heightMapOffsetX = lodOffsetX - lodIncrement;
	auto heightMapOffsetY = lodOffsetY - lodIncrement;

	auto borderSize = tileSize;
	auto borderVertices = borderSize + 1;
	auto borderOffsetX = tileOffsetX;
	auto borderOffsetY = tileOffsetY;

	auto borderHeightMapVertices = borderVertices + 2;
	auto borderHeightMapOffsetX = borderOffsetX - 1;
	auto borderHeightMapOffsetY = borderOffsetY - 1;

	// create temporary heightmap with border so we can calculate normals
	float* map = new float[heightMapVertices * heightMapVertices];
	float* ptr = map;

	for (auto iy = 0; iy < heightMapVertices; iy++) {
		auto wy = heightMapOffsetY + iy * lodIncrement;

		for (auto ix = 0; ix < heightMapVertices; ix++) {
			auto wx = heightMapOffsetX + ix * lodIncrement;
			*ptr++ = noisemap->getNoise(wx, wy);
		}
	}

	// add LOD vertices
#define HEIGHT(x, y) map[(y + 1) * heightMapVertices + (x + 1)]

	for (auto iy = 0; iy < lodVertices; iy++) {
		auto wy = lodOffsetY + iy * lodIncrement;

		for (auto ix = 0; ix < lodVertices; ix++) {
			auto wx = lodOffsetX + ix * lodIncrement;

			vertices.push_back(OtTerrainVertex(
				glm::vec3(wx, HEIGHT(ix, iy), wy),
				glm::normalize(glm::vec3(
					HEIGHT(ix - 1, iy) - HEIGHT(ix + 1, iy),
					2.0,
					HEIGHT(ix, iy - 1) - HEIGHT(ix, iy + 1)))));
		}
	}

#undef HEIGHT

	// cleanup
	delete [] map;

	// add LOD triangles
	for (auto iy = 0; iy < lodSegments; iy++) {
		for (auto ix = 0; ix < lodSegments; ix ++) {
			auto a = ix + lodVertices * iy;
			auto b = ix + lodVertices * (iy + 1);
			auto c = (ix + 1) + lodVertices * (iy + 1);
			auto d = (ix + 1) + lodVertices * iy;

			triangles.push_back(a);
			triangles.push_back(b);
			triangles.push_back(d);

			triangles.push_back(b);
			triangles.push_back(c);
			triangles.push_back(d);
		}
	}

	// add border vertices
	map = new float[borderHeightMapVertices * 3];

#define HEIGHT(x, y) map[(y + 1) * 3 + (x + 1)]

	//
	// LEFT BORDER
	//

	// create left border heightmap
	ptr = map;

	for (auto iy = 0; iy < borderHeightMapVertices; iy++) {
		auto wy = borderHeightMapOffsetY + iy;

		for (auto ix = 0; ix < 3; ix++) {
			auto wx = borderHeightMapOffsetX + ix;
			*ptr++ = noisemap->getNoise(wx, wy);
		}
	}

	// add left border vertices
	auto index = vertices.size();

	for (auto iy = 0; iy < borderVertices; iy++) {
		vertices.push_back(OtTerrainVertex(
			glm::vec3(borderOffsetX, HEIGHT(0, iy), borderOffsetY + iy),

			glm::normalize(glm::vec3(
				HEIGHT(-1, iy) - HEIGHT(1, iy),
				2.0,
				HEIGHT(0, iy - 1) - HEIGHT(0, iy + 1)))));
	}

	// add left border triangles
	triangles.push_back(index);
	triangles.push_back(index + 1);
	triangles.push_back(0);

	for (auto s = 0; s < lodSegments; s++) {
		auto borderIndex = index + 1 + s * lodIncrement;
		auto lodIndex = s * lodVertices;

		triangles.push_back(lodIndex);
		lodIndex += lodVertices;
		triangles.push_back(borderIndex);
		triangles.push_back(lodIndex);

		for (auto i = 0; i < lodIncrement; i++) {
			triangles.push_back(borderIndex++);
			triangles.push_back(borderIndex);
			triangles.push_back(lodIndex);
		}
	}

	triangles.push_back(index + borderVertices - 2);
	triangles.push_back(index + borderVertices - 1);
	triangles.push_back(lodSegments * lodVertices);

	//
	// RIGHT BORDER
	//

	// create right border heightmap
	ptr = map;

	for (auto iy = 0; iy < borderHeightMapVertices; iy++) {
		auto wy = borderHeightMapOffsetY + iy;

		for (auto ix = 0; ix < 3; ix++) {
			auto wx = borderHeightMapOffsetX + borderSize + ix;
			*ptr++ = noisemap->getNoise(wx, wy);
		}
	}

	// add right border vertices
	index = vertices.size();

	for (auto iy = 0; iy < borderVertices; iy++) {
		vertices.push_back(OtTerrainVertex(
			glm::vec3(borderOffsetX + borderSize, HEIGHT(0, iy), borderOffsetY + iy),
			glm::normalize(glm::vec3(
				HEIGHT(-1, iy) - HEIGHT(1, iy),
				2.0,
				HEIGHT(0, iy - 1) - HEIGHT(0, iy + 1)))));
	}

	// add right border triangles
	triangles.push_back(index);
	triangles.push_back(lodSegments);
	triangles.push_back(index + 1);

	for (auto s = 0; s < lodSegments; s++) {
		auto borderIndex = index + 1 + s * lodIncrement;
		auto lodIndex = lodSegments + s * lodVertices;

		triangles.push_back(lodIndex);
		lodIndex += lodVertices;
		triangles.push_back(lodIndex);
		triangles.push_back(borderIndex);

		for (auto i = 0; i < lodIncrement; i++) {
			triangles.push_back(borderIndex++);
			triangles.push_back(lodIndex);
			triangles.push_back(borderIndex);
		}
	}

	triangles.push_back(index + borderSize);
	triangles.push_back(index + borderSize - 1);
	triangles.push_back(lodVertices * lodVertices - 1);

#undef HEIGHT
#define HEIGHT(x, y) map[(y + 1) * borderHeightMapVertices + (x + 1)]

	//
	// TOP BORDER
	//

	// create top border heightmap
	ptr = map;

	for (auto iy = 0; iy < 3; iy++) {
		auto wy = borderHeightMapOffsetY + iy;

		for (auto ix = 0; ix < borderHeightMapVertices; ix++) {
			auto wx = borderHeightMapOffsetX + ix;
			*ptr++ = noisemap->getNoise(wx, wy);
		}
	}

	// add top border vertices
	index = vertices.size();

	for (auto ix = 0; ix < borderVertices; ix++) {
		vertices.push_back(OtTerrainVertex(
			glm::vec3(borderOffsetX + ix, HEIGHT(ix, 0), borderOffsetY),
			glm::normalize(glm::vec3(
				HEIGHT(ix - 1, 0) - HEIGHT(ix + 1, 0),
				2.0,
				HEIGHT(ix, -1) - HEIGHT(ix, 1)))));
	}

	// add top border triangles
	triangles.push_back(index);
	triangles.push_back(0);
	triangles.push_back(index + 1);

	for (auto s = 0; s < lodSegments; s++) {
		auto borderIndex = index + 1 + s * lodIncrement;
		auto lodIndex = s;

		triangles.push_back(lodIndex++);
		triangles.push_back(lodIndex);
		triangles.push_back(borderIndex);

		for (auto i = 0; i < lodIncrement; i++) {
			triangles.push_back(borderIndex++);
			triangles.push_back(lodIndex);
			triangles.push_back(borderIndex);
		}
	}

	triangles.push_back(index + borderVertices - 2);
	triangles.push_back(lodSegments);
	triangles.push_back(index + borderVertices - 1);

	//
	// BOTTOM BORDER
	//

	// create bottom border heightmap
	ptr = map;

	for (auto iy = 0; iy < 3; iy++) {
		auto wy = borderHeightMapOffsetY + borderSize + iy;

		for (auto ix = 0; ix < borderHeightMapVertices; ix++) {
			auto wx = borderHeightMapOffsetX + ix;
			*ptr++ = noisemap->getNoise(wx, wy);
		}
	}

	// add bottom border vertices
	index = vertices.size();

	for (auto ix = 0; ix < borderVertices; ix++) {
		vertices.push_back(OtTerrainVertex(
			glm::vec3(borderOffsetX + ix, HEIGHT(ix, 0), borderOffsetY + borderSize),
			glm::normalize(glm::vec3(
				HEIGHT(ix - 1, 0) - HEIGHT(ix + 1, 0),
				2.0,
				HEIGHT(ix, -1) - HEIGHT(ix, 1)))));
	}

	// add bottom border triangles
	triangles.push_back(index);
	triangles.push_back(index + 1);
	triangles.push_back(lodSegments * lodVertices);

	for (auto s = 0; s < lodSegments; s++) {
		auto borderIndex = index + 1 + s * lodIncrement;
		auto lodIndex = lodSegments * lodVertices + s;

		triangles.push_back(lodIndex++);
		triangles.push_back(borderIndex);
		triangles.push_back(lodIndex);

		for (auto i = 0; i < lodIncrement; i++) {
			triangles.push_back(borderIndex++);
			triangles.push_back(borderIndex);
			triangles.push_back(lodIndex);
		}
	}

	triangles.push_back(lodVertices * lodVertices - 1);
	triangles.push_back(index + borderVertices - 2);
	triangles.push_back(index + borderVertices - 1);

	// cleanup
	delete [] map;

	// create buffers
	bgfx::VertexLayout layout;

	layout.begin()
		.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
		.add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float)
		.end();

	vertexBuffer = bgfx::createVertexBuffer(bgfx::makeRef(vertices.data(), sizeof(OtTerrainVertex) * vertices.size()), layout);
	indexBuffer = bgfx::createIndexBuffer(bgfx::makeRef(triangles.data(), sizeof(uint32_t) * triangles.size()), BGFX_BUFFER_INDEX32);
}
