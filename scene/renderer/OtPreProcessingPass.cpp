//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtSceneRenderer.h"


//
//	OtSceneRenderer::renderPreProcessingPass
//

void OtSceneRenderer::renderPreProcessingPass(OtScene* scene, OtEntity selected) {
	// determine the camera's frustum in worldspace
	frustum = OtFrustum(viewProjectionMatrix);

	// reset flags
	hasOpaqueEntities = false;
	hasTransparentEntities = false;
	hasSkyEntities = false;
	hasTerrainEntities = false;
	renderEntityHighlight = false;

	// reset list of visible entities
	visibleEntities.clear();

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
		}
	});

	// see if we have terrain objects
	for (auto [entity, component] : scene->view<OtTerrainComponent>().each()) {
		preprocessTerrain(scene, entity);
		hasTerrainEntities = true;
	};

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
//	OtSceneRenderer::preprocessTerrain
//

void OtSceneRenderer::preprocessTerrain(OtScene* scene, OtEntity entity) {
	auto terrain = scene->getComponent<OtTerrainComponent>(entity).terrain;

	// refresh heightmap and normalmap if the properties have changed
	if (terrain->heights.dirty) {
		terrain->heights.update(tileableFbm, normalMapper);
	}
}
