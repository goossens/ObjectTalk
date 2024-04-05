//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "glm/glm.hpp"

#include "OtFrameBuffer.h"
#include "OtGbuffer.h"

#include "OtCamera.h"
#include "OtScene.h"


//
//	OtSceneRendererContext
//

class OtSceneRendererContext {
public:
	// constructor
	OtSceneRendererContext(OtCamera& c, OtGbuffer& db, OtFrameBuffer& cb, OtScene* s, const glm::vec4& clp=glm::vec4(0.0f), bool water=true) :
		camera(c),
		deferedBuffer(db),
		compositeBuffer(cb),
		scene(s),
		clippingPlane(clp) {

		// reset flags
		hasDirectionalLighting = false;
		hasImageBasedLighting = false;
		hasOpaqueEntities = false;
		hasOpaqueGeometries = false;
		hasOpaqueModels = false;
		hasTerrainEntities = false;
		hasSkyEntities = false;
		hasTransparentEntities = false;
		hasWaterEntities = false;
		hasParticlesEntities = false;

		iblEntity = OtEntityNull;
		waterEntity = OtEntityNull;

		// check entities and set flags
		scene->eachEntityDepthFirst([&](OtEntity entity) {
			if (scene->hasComponent<OtDirectionalLightComponent>(entity) && scene->hasComponent<OtTransformComponent>(entity)) {
				auto& light = scene->getComponent<OtDirectionalLightComponent>(entity);
				auto& transform = scene->getComponent<OtTransformComponent>(entity);

				hasDirectionalLighting = true;
				directionalLightDirection = glm::normalize(transform.getTransform()[3]);
				directionalLightColor = light.color;
				directionalLightAmbient = light.ambient;
				castShadow = light.castShadow;
			}

			if (scene->hasComponent<OtSkyComponent>(entity)) {
				auto& sky = scene->getComponent<OtSkyComponent>(entity);

				hasDirectionalLighting = true;
				directionalLightDirection = glm::normalize(sky.getDirectionToSun());
				directionalLightColor = glm::vec3(0.2f + std::clamp(sky.elevation / 10.0f, 0.0f, 0.8f));
				directionalLightAmbient = std::clamp((sky.elevation + 6.0f) / 200.0f, 0.0f, 0.2f);
				castShadow = sky.castShadow;
			}

			if (scene->hasComponent<OtIblComponent>(entity)) {
				hasImageBasedLighting |= scene->getComponent<OtIblComponent>(entity).cubemap.isReady();
				iblEntity = entity;
			}

			if (scene->hasComponent<OtGeometryComponent>(entity)) {
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

			if (scene->hasComponent<OtSkyComponent>(entity) || scene->hasComponent<OtSkyBoxComponent>(entity)) {
				hasSkyEntities = true;
			}

			if (water && scene->hasComponent<OtWaterComponent>(entity)) {
				hasTransparentEntities = true;
				hasWaterEntities = true;
				waterEntity = entity;
			}

			if (scene->hasComponent<OtParticlesComponent>(entity)) {
				hasParticlesEntities = true;
			}
		});
	}

	// camera information
	OtCamera& camera;

	// rendering buffers
	OtGbuffer& deferedBuffer;
	OtFrameBuffer& compositeBuffer;
	OtFrameBuffer* output;

	// scene to render
	OtScene* scene;

	// clipping plane
	glm::vec4 clippingPlane;

	// rendering flags
	bool hasDirectionalLighting;
	bool hasImageBasedLighting;
	bool hasOpaqueEntities;
	bool hasOpaqueGeometries;
	bool hasOpaqueModels;
	bool hasTerrainEntities;
	bool hasSkyEntities;
	bool hasTransparentEntities;
	bool hasWaterEntities;
	bool hasParticlesEntities;

	// directional light information
	glm::vec3 directionalLightDirection = glm::vec3(0.0f);
	glm::vec3 directionalLightColor = glm::vec3(0.0f);
	float directionalLightAmbient = 0.0f;
	bool castShadow = false;

	// key entities
	OtEntity iblEntity = OtEntityNull;
	OtEntity waterEntity = OtEntityNull;
};
