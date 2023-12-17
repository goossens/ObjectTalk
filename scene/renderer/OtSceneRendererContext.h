//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
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
	OtSceneRendererContext(OtCamera& c, OtGbuffer& db, OtFrameBuffer& cb, OtFrameBuffer& ppb, OtScene* s, const glm::vec4& clp=glm::vec4(0.0f), bool water=true) :
		camera(c),
		deferedBuffer(db),
		compositeBuffer(cb),
		postProcessBuffer(ppb),
		scene(s),
		clippingPlane(clp) {

		// reset flags
		hasOpaqueEntities = false;
		hasOpaqueGeometries = false;
		hasOpaqueModels = false;
		hasTerrainEntities = false;
		hasSkyEntities = false;
		hasTransparentEntities = false;
		hasWaterEntities = false;

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

			if (scene->hasComponent<OtSkyComponent>(entity) ||
				scene->hasComponent<OtSkyBoxComponent>(entity) ||
				scene->hasComponent<OtSkySphereComponent>(entity)) {
				hasSkyEntities = true;
			}

			if (water && scene->hasComponent<OtWaterComponent>(entity)) {
				hasTransparentEntities = true;
				hasWaterEntities = true;
			}
		});
	}

	// camera information
	OtCamera& camera;

	// rendering buffers
	OtGbuffer& deferedBuffer;
	OtFrameBuffer& compositeBuffer;
	OtFrameBuffer& postProcessBuffer;

	// scene to render
	OtScene* scene;

	// clipping plane
	glm::vec4 clippingPlane;

	// rendering flags
	bool hasOpaqueEntities;
	bool hasOpaqueGeometries;
	bool hasOpaqueModels;
	bool hasTerrainEntities;
	bool hasSkyEntities;
	bool hasTransparentEntities;
	bool hasWaterEntities;
};