//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "glm/glm.hpp"

#include "OtCamera.h"
#include "OtCascadedShadowMap.h"
#include "OtPass.h"

#include "OtImageBasedLighting.h"
#include "OtScene.h"


//
//	OtSceneRendererContext
//

class OtSceneRendererContext {
public:
	// constructors
	OtSceneRendererContext(OtCamera c, OtScene* s, OtImageBasedLighting* i, OtCascadedShadowMap* sm, const glm::vec4& clp=glm::vec4(0.0f)) :
			camera(c),
			scene(s),
			ibl(i),
			csm(sm),
			clippingPlane(clp) {
		initialize();
	}

	// camera information
	OtCamera camera;

	// scene to render
	OtScene* scene;

	// rendering pass
	OtPass* pass;

	// image base lighting
	OtImageBasedLighting* ibl;

	// shadows
	OtCascadedShadowMap* csm;

	// clipping plane
	glm::vec4 clippingPlane;

	// rendering flags
	bool hasImageBasedLighting;
	bool hasDirectionalLighting;
	bool hasPointLighting;
	bool hasOpaqueEntities;
	bool hasOpaqueGeometries;
	bool hasOpaqueModels;
	bool hasTerrainEntities;
	bool hasSkyEntities;
	bool hasTransparentEntities;
	bool hasWaterEntities;
	bool hasParticlesEntities;
	bool renderingShadow = false;

	// directional light information
	glm::vec3 directionalLightDirection = glm::vec3(0.0f);
	glm::vec3 directionalLightColor = glm::vec3(0.0f);
	float directionalLightAmbient = 0.0f;
	bool renderDirectionalLight = false;
	bool castShadow = false;

	// key entities
	OtEntity iblEntity = OtEntityNull;
	OtEntity waterEntity = OtEntityNull;

private:
	// initialize context
	void initialize();
};
