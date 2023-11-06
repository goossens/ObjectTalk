//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <vector>

#include "glm/glm.hpp"

#include "OtHash.h"

#include "OtVertex.h"

#include "OtSceneRenderer.h"


//
//	OtSceneRenderer::renderTerrainPass
//

void OtSceneRenderer::renderTerrainPass(OtScene* scene) {
	// setup pass
	OtPass pass;
	pass.setClear(false, false);
	pass.setRectangle(0, 0, width, height);
	pass.setFrameBuffer(gbuffer);
	pass.setTransform(viewMatrix, projectionMatrix);

	// render each terrain tile
	for (auto& tile : terrainTiles) {
		renderTerrainTile(pass, tile);
	}
}


//
//	OtSceneRenderer::renderTerrainTile
//

void OtSceneRenderer::renderTerrainTile(OtPass& pass, TerrainTile& tile) {
	// determine the program
	OtShaderProgram* program;

	if (tile.material.isKindOf<OtPbrMaterialClass>()) {
		program = &geometryPbrProgram;

	} else if (tile.material.isKindOf<OtTerrainMaterialClass>()) {
		program = &geometryTerrainProgram;
	}

	// submit the material information
	submitMaterialUniforms(tile.material);

}


//
//	OtSceneRenderer::submitTerrainGeometry
//

void OtSceneRenderer::submitTerrainGeometry(TerrainTile& tile) {
	// vertext and index buffers
	OtVertexBuffer* vertices;
	OtIndexBuffer* indices;
	auto size = (tile.terrain->tileSize / (1 << (tile.lod - 1))) + 1;

	// see if these vertices already exist
	auto hash = OtHash(tile.terrain->tileSize, tile.lod);

	if (terrainVertices.has(hash)) {
		vertices = &terrainVertices.get(hash);

	} else {
		// we have to create new vertices
		vertices = &terrainVertices.emplace(hash);
		glm::vec2* points = new glm::vec2[size * size];
		glm::vec2* p = points;
		float step = 1.0f / size;

		for (auto z = 0; z < size; z++) {
			for (auto x = 0; x < size; x++) {
				*p++ = glm::vec2(x * step, z * step);
			}
		}

		vertices->set(points, size * size, OtVertexUv::getLayout());

		// cleanup
		delete [] points;
	}

	// see if the indices already exist
	hash = OtHash(tile.terrain->tileSize, tile.lod, tile.northLod, tile.eastLod, tile.southLod, tile.westLod);

	if (terrainIndices.has(hash)) {
		indices = &terrainIndices.get(hash);

	} else {
		// we have to create new indices
		indices = &terrainIndices.emplace(hash);

		// determine spacing
		int maxSpacing = tile.terrain->tileSize >> 1;
		int northSpacing = std::clamp(1 << (tile.northLod - tile.lod), 1, maxSpacing);
		int eastSpacing = std::clamp(1 << (tile.eastLod - tile.lod), 1, maxSpacing);
		int southSpacing = std::clamp(1 << (tile.southLod - tile.lod), 1, maxSpacing);
		int westSpacing = std::clamp(1 << (tile.westLod - tile.lod), 1, maxSpacing);

		// limits
		int top = 0;
		int right = size + 1;
		int bottom = size + 1;
		int left = 0;

		// triangle indices
		std::vector<uint32_t> triangles;


	}
}
