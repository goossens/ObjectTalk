//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>
#include <cmath>

#include "OtHash.h"

#include "OtSceneRenderer.h"


//
//	OtSceneRenderer::renderPreProcessingPass
//

void OtSceneRenderer::renderPreProcessingPass(OtScene* scene, OtEntity selected) {
	// determine the camera's frustum in worldspace
	frustum = OtFrustum(viewProjectionMatrix);

	// reset flags
	renderEntityHighlight = false;
	hasTerrainEntities = false;
	hasSkyEntities = false;

	// reset lists of visible entities
	visibleEntities.clear();
	terrainTiles.clear();

	// build lists of visible entities
	scene->eachEntityDepthFirst([&](OtEntity entity) {
		if (scene->hasComponent<OtGeometryComponent>(entity) && scene->hasComponent<OtTransformComponent>(entity) && scene->hasComponent<OtMaterialComponent>(entity)) {
			preprocessSingleInstanceGeometry(scene, entity, entity == selected);

		} else if (scene->hasComponent<OtGeometryComponent>(entity) && scene->hasComponent<OtInstancingComponent>(entity) && scene->hasComponent<OtMaterialComponent>(entity)) {
			preprocessMultipleInstanceGeometry(scene, entity, entity == selected);

		} else if (scene->hasComponent<OtModelComponent>(entity) && scene->hasComponent<OtTransformComponent>(entity)) {
			preprocessSingleInstanceModel(scene, entity, entity == selected);

		} else if (scene->hasComponent<OtModelComponent>(entity) && scene->hasComponent<OtInstancingComponent>(entity)) {
			preprocessMultipleInstanceModel(scene, entity, entity == selected);

		} else if (scene->hasComponent<OtTerrainComponent>(entity) && scene->hasComponent<OtMaterialComponent>(entity)) {
			preprocessTerrainTiles(scene, entity);
		}
	});

	// see if we have any sky objects
	for (auto [entity, component] : scene->view<OtSkyComponent>().each()) {
		hasSkyEntities = true;
	};

	for (auto [entity, component] : scene->view<OtSkyBoxComponent>().each()) {
		if (component.cubemap.isReady()) {
			hasSkyEntities = true;
		}
	};

	for (auto [entity, component] : scene->view<OtSkySphereComponent>().each()) {
		if (component.texture.isReady()) {
			hasSkyEntities = true;
		}
	};
}


//
//	OtSceneRenderer::preprocessSingleInstanceGeometry
//

void OtSceneRenderer::preprocessSingleInstanceGeometry(OtScene* scene, OtEntity entity, bool selected) {
	// get the geometry's AABB in worldspace
	auto& geometry = scene->getComponent<OtGeometryComponent>(entity);
	auto& transform = scene->getComponent<OtTransformComponent>(entity);
	auto aabb = geometry.geometry->getAABB().transform(transform.getTransform());

	// is this entity visible
	if (frustum.isVisibleAABB(aabb)) {
		auto& visibleEntity = visibleEntities.emplace_back(entity);

		if (geometry.transparent) {
			visibleEntity.transparent = true;
			hasTransparentEntities = true;

		} else {
			hasOpaqueEntities = true;
		}

		// activate highlighting if this entity is selected
		if (selected) {
			renderEntityHighlight = true;
		}
	}
}


//
//	OtSceneRenderer::preprocessMultipleInstanceGeometry
//

void OtSceneRenderer::preprocessMultipleInstanceGeometry(OtScene* scene, OtEntity entity, bool selected) {
	// build list of visible instances
	auto& geometry = scene->getComponent<OtGeometryComponent>(entity);
	auto& instances = scene->getComponent<OtInstancingComponent>(entity);
	instances.determineVisibleInstances(frustum, geometry.geometry->getAABB());

	// are any instances visible
	if (instances.visibleTransforms.size()) {
		// if so, mark this entity for rendering
		auto& visibleEntity = visibleEntities.emplace_back(entity);
		visibleEntity.instanced = true;

		if (geometry.transparent) {
			visibleEntity.transparent = true;
			hasTransparentEntities = true;

		} else {
			hasOpaqueEntities = true;
		}

		// activate highlighting if this entity is selected
		if (selected) {
			renderEntityHighlight = true;
		}
	}
}


//
//	OtSceneRenderer::preprocessSingleInstanceModel
//

void OtSceneRenderer::preprocessSingleInstanceModel(OtScene* scene, OtEntity entity, bool selected) {
	// see if model is ready
	auto& model = scene->getComponent<OtModelComponent>(entity).model;

	if (model.isReady()) {
		// get the geometry's AABB in worldspace
		auto& transform = scene->getComponent<OtTransformComponent>(entity);
		auto aabb = model->getAABB().transform(transform.getTransform());

		// is this one visible
		if (frustum.isVisibleAABB(aabb)) {
			auto& visibleEntity = visibleEntities.emplace_back(entity);
			visibleEntity.model = true;
			hasOpaqueEntities = true;

			// activate highlighting if this entity is selected
			if (selected) {
				renderEntityHighlight = true;
			}
		}
	}
}


//
//	OtSceneRenderer::preprocessMultipleInstanceModel
//

void OtSceneRenderer::preprocessMultipleInstanceModel(OtScene* scene, OtEntity entity, bool selected) {
	// build list of visible instances
	auto& model = scene->getComponent<OtModelComponent>(entity).model;
	auto& instances = scene->getComponent<OtInstancingComponent>(entity);
	instances.determineVisibleInstances(frustum, model->getAABB());

	// are any instances visible
	if (instances.visibleTransforms.size()) {
		// if so, mark this entity for rendering
		auto& visibleEntity = visibleEntities.emplace_back(entity);
		visibleEntity.model = true;
		visibleEntity.instanced = true;
		hasOpaqueEntities = true;

		// activate highlighting if this entity is selected
		if (selected) {
			renderEntityHighlight = true;
		}
	}
}


//
//	OtSceneRenderer::preprocessTerrainTiles
//

void OtSceneRenderer::preprocessTerrainTiles(OtScene* scene, OtEntity entity) {
	// set flag
	hasTerrainEntities = true;

	// get components
	auto& terrain = scene->getComponent<OtTerrainComponent>(entity);
	auto material = scene->getComponent<OtMaterialComponent>(entity).material;

	// determine number of tiles visible from center
	auto visibleFromCenter = (int) std::ceil(terrain.maxViewingDist / terrain.tileSize);

	// determine center tile
	auto centerX = (int) std::floor(cameraPosition.x / terrain.maxViewingDist);
	auto centerZ = (int) std::floor(cameraPosition.z / terrain.maxViewingDist);

	// get vertical limits
	auto minHeight = terrain.vOffset * terrain.vScale;
	auto maxHeight = (1.0f + terrain.vOffset) * terrain.vScale;

	// index of all tiles and their LOD
	std::unordered_map<size_t, int> index;

	// process all possible visible tiles
	for (auto zOffset = -visibleFromCenter; zOffset <= visibleFromCenter; zOffset++) {
		for (auto xOffset = -visibleFromCenter; xOffset <= visibleFromCenter; xOffset++) {
			// get tile coordinates
			auto x = centerX + xOffset;
			auto z = centerZ + zOffset;

			// ensure tile is visible in camera frustum
			OtAABB aabb;
			float sx = x * terrain.tileSize;
			float sz = z * terrain.tileSize;
			aabb.addPoint(glm::vec3(sx, minHeight, sz));
			aabb.addPoint(glm::vec3(sx + terrain.tileSize, maxHeight, sz + terrain.tileSize));

			if (frustum.isVisibleAABB(aabb)) {
				// determine LOD
				int distanceInTiles = std::ceil(glm::distance(glm::vec2(centerX, centerZ), glm::vec2(centerX, centerZ)));
				int lod = std::clamp(distanceInTiles, 1, 7);
				terrainTiles.emplace_back(&terrain, material, x, z, lod);
				index[OtHash(x, z)] = lod;
			}
		}
	}

	// set LOD for tile neighbors
	auto updateNeighbor = [&](int& lod, int x, int z) {
		auto hash = OtHash(x, z);

		if (index.count(hash)) {
			lod = index[hash];
		}
	};

	for (auto& tile : terrainTiles) {
		updateNeighbor(tile.northLod, tile.x, tile.z - 1);
		updateNeighbor(tile.eastLod, tile.x + 1, tile.z);
		updateNeighbor(tile.southLod, tile.x, tile.z + 1);
		updateNeighbor(tile.westLod, tile.x - 1, tile.z);
	}
}
