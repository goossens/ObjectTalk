//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtSceneRendererContext.h"


//
//	OtSceneRendererContext::initialize
//

void OtSceneRendererContext::initialize() {
	// reset flags
	hasImageBasedLighting = false;
	hasDirectionalLighting = false;
	hasPointLighting = false;
	hasOpaqueEntities = false;
	hasOpaqueGeometries = false;
	hasOpaqueModels = false;
	hasTerrainEntities = false;
	hasSkyEntities = false;
	hasTransparentEntities = false;
	hasWaterEntities = false;
	hasParticlesEntities = false;

	renderDirectionalLight = false;
	castShadow = false;

	iblEntity = OtEntityNull;
	waterEntity = OtEntityNull;

	// check entities, collect data and set flags
	scene->eachEntityBreadthFirst([&](OtEntity entity) {
		// process image based lighting
		if (scene->hasComponent<OtIblComponent>(entity)) {
			hasImageBasedLighting |= scene->getComponent<OtIblComponent>(entity).cubemap.isReady();
			iblEntity = entity;
		}

		// process directional light
		if (scene->hasComponent<OtDirectionalLightComponent>(entity)) {
			auto& light = scene->getComponent<OtDirectionalLightComponent>(entity);

			hasDirectionalLighting = true;
			directionalLightDirection = light.getDirectionToLight();
			directionalLightColor = light.color;
			directionalLightAmbient = light.ambient;
			renderDirectionalLight = true;
			castShadow = light.castShadow;
		}

		// process procedural sky
		if (scene->hasComponent<OtSkyComponent>(entity)) {
			auto& sky = scene->getComponent<OtSkyComponent>(entity);

			hasDirectionalLighting = true;
			directionalLightDirection = sky.getDirectionToSun();
			directionalLightColor = glm::vec3(0.2f + std::clamp(sky.elevation / 10.0f, 0.0f, 0.8f));
			directionalLightAmbient = std::clamp((sky.elevation + 6.0f) / 200.0f, 0.0f, 0.2f);
			renderDirectionalLight = false;
			castShadow = sky.castShadow;
		}

		// process point lighting
		if (scene->hasComponent<OtPointLightComponent>(entity)) {
			hasPointLighting = true;
		}

		// process all geometry entities
		if (scene->hasComponent<OtGeometryComponent>(entity)) {
			bool transparent = scene->getComponent<OtGeometryComponent>(entity).transparent;
			hasOpaqueEntities |= !transparent;
			hasOpaqueGeometries |= !transparent;
			hasTransparentEntities |= transparent;
		}

		// process all model entities
		if (scene->hasComponent<OtModelComponent>(entity)) {
			hasOpaqueEntities = true;
			hasOpaqueModels = true;
		}

		// process all terrain entities
		if (scene->hasComponent<OtTerrainComponent>(entity)) {
			hasOpaqueEntities = true;
			hasTerrainEntities = true;
		}

		// process all sky entities
		if (scene->hasComponent<OtSkyComponent>(entity) || scene->hasComponent<OtSkyBoxComponent>(entity)) {
			hasSkyEntities = true;
		}

		// process all water entities
		if (scene->hasComponent<OtWaterComponent>(entity)) {
			hasTransparentEntities = true;
			hasWaterEntities = true;
			waterEntity = entity;
		}

		// process all particle entities
		if (scene->hasComponent<OtParticlesComponent>(entity)) {
			hasParticlesEntities = true;
		}
	});
}
