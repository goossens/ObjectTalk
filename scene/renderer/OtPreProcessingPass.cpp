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

void OtSceneRenderer::renderPreProcessingPass(OtScene* scene) {
	// reset flags
	hasOpaqueEntities = false;
	hasOpaqueGeometries = false;
	hasOpaqueModels = false;
	hasTerrainEntities = false;
	hasWaterEntities = false;
	hasTransparentEntities = false;
	hasSkyEntities = false;

	// check entities and set flags
	scene->eachEntityDepthFirst([&](OtEntity entity) {
		if (scene->hasComponent<OtGeometryComponent>(entity) && scene->hasComponent<OtMaterialComponent>(entity)) {
			bool transparent = scene->getComponent<OtGeometryComponent>(entity).transparent;
			hasOpaqueEntities |= !transparent;
			hasOpaqueGeometries |= !transparent;
			hasTransparentEntities |= transparent;
		}

		if (scene->hasComponent<OtModelComponent>(entity)) {
			hasOpaqueEntities = true;
			hasOpaqueModels = true;
		}

		if (scene->hasComponent<OtTerrainComponent>(entity)) {
			hasOpaqueEntities = true;
			hasTerrainEntities = true;
		}

		if (scene->hasComponent<OtWaterComponent>(entity)) {
			hasOpaqueEntities = true;
			hasWaterEntities = true;
		}

		if (scene->hasComponent<OtSkyComponent>(entity) ||
			scene->hasComponent<OtSkyBoxComponent>(entity) ||
			scene->hasComponent<OtSkySphereComponent>(entity)) {
			hasSkyEntities = true;
		}
	});
}
