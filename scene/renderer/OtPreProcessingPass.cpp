//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtSceneRenderer.h"

#include "OtFormat.h"
#include "OtLog.h"

//
//	OtSceneRenderer::renderPreProcessingPass
//

void OtSceneRenderer::renderPreProcessingPass(OtScene* scene) {
	// determine the camera's frustum in worldspace
	frustum = OtFrustum(viewProjectionMatrix);

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
			auto aabb = geometry.geometry->getAABB().transform(transform.getTransform());

			// is this one visible
			if (frustum.isVisibleAABB(aabb)) {
				if (geometry.transparent) {
					transparentGeometries.push_back(entity);

				} else {
					opaqueGeometries.push_back(entity);
				}

			}
		}
	});

	// process all models
	scene->eachEntityDepthFirst([&](OtEntity entity) {
		if (scene->hasComponent<OtModelComponent>(entity) &&
			scene->hasComponent<OtTransformComponent>(entity)) {

			// is this one visible
			auto& model = scene->getComponent<OtModelComponent>(entity);

			if (frustum.isVisibleAABB(model.model->getAABB())) {
				opaqueModels.push_back(entity);
			}
		}
	});

	// set flags
	hasOpaqueObjects = opaqueGeometries.size() || opaqueModels.size();
	hasTransparentObjects = transparentGeometries.size() || transparentModels.size();

	// see if we have any sky objects
	hasSkyObjects = false;

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

	OtLogDebug(OtFormat("%d %d %d %d", opaqueGeometries.size(), opaqueModels.size(), transparentGeometries.size(), transparentModels.size()));
}
