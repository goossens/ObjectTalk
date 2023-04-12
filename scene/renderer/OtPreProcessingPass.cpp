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

	// reset highlighting flag
	renderEntityHighlight = false;

	// build lists of visible geometries and models (both opaque and transparent)
	opaqueGeometries.clear();
	opaqueModels.clear();
	transparentGeometries.clear();
	transparentModels.clear();

	// process all geometries
	scene->eachEntityDepthFirst([&](OtEntity entity) {
		if (scene->hasComponent<OtGeometryComponent>(entity) &&
			scene->hasComponent<OtTransformComponent>(entity) &&
			scene->hasComponent<OtMaterialComponent>(entity)) {

			// get the geometry's AABB in worldspace
			auto& geometry = scene->getComponent<OtGeometryComponent>(entity);
			auto& transform = scene->getComponent<OtTransformComponent>(entity);
			auto aabb = geometry.geometry->getAABB();
			aabb.transform(transform.getTransform());

			// is this one visible
			if (frustum.isVisibleAABB(aabb)) {
				// add it to the appropriate list
				if (geometry.transparent) {
					transparentGeometries.push_back(entity);

				} else {
					opaqueGeometries.push_back(entity);
				}

				// activate highlighting if this entity is selected
				if (entity == selected) {
					renderEntityHighlight = true;
				}
			}
		}
	});

	// process all models
	scene->eachEntityDepthFirst([&](OtEntity entity) {
		if (scene->hasComponent<OtModelComponent>(entity) &&
			scene->hasComponent<OtTransformComponent>(entity)) {

			// see if model is ready
			auto& model = scene->getComponent<OtModelComponent>(entity).model;

			if (model.isReady()) {
				// get the geometry's AABB in worldspace
				auto& transform = scene->getComponent<OtTransformComponent>(entity);
				auto aabb = model->getAABB();
				aabb.transform(transform.getTransform());

				// is this one visible
				if (frustum.isVisibleAABB(aabb)) {
					// add to list
					opaqueModels.push_back(entity);

					// activate highlighting if this entity is selected
					if (entity == selected) {
						renderEntityHighlight = true;
					}
				}
			}
		}
	});

	// set flags
	hasOpaqueObjects = opaqueGeometries.size() || opaqueModels.size();
	hasTransparentObjects = transparentGeometries.size() || transparentModels.size();

	// see if we have any sky objects
	hasSkyObjects = false;

	for (auto [entity, component] : scene->view<OtSkyComponent>().each()) {
		hasSkyObjects = true;
	};

	for (auto [entity, component] : scene->view<OtSkyBoxComponent>().each()) {
		if (component.cubemap.isReady()) {
			hasSkyObjects = true;
		}
	};

	for (auto [entity, component] : scene->view<OtSkySphereComponent>().each()) {
		if (component.texture.isReady()) {
			hasSkyObjects = true;
		}
	};
}
