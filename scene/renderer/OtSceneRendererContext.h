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

#include "OtTextureAsset.h"

#include "OtCamera.h"
#include "OtCascadedShadowMap.h"
#include "OtGrass.h"
#include "OtMaterial.h"
#include "OtPass.h"
#include "OtSampler.h"
#include "OtUniformMat4.h"
#include "OtUniformVec4.h"

#include "OtImageBasedLighting.h"
#include "OtScene.h"
#include "OtTerrain.h"


//
//	OtSceneRendererContext
//

class OtSceneRendererContext {
public:
	// initialize context
	void initialize(OtCamera c, OtScene* s, OtImageBasedLighting* i, OtCascadedShadowMap* sm);

	// utility functions
	void submitClippingUniforms();
	void submitLightingUniforms();
	void submitShadowUniforms();
	void submitAlbedoUniforms(OtMaterial& material);
	void submitMaterialUniforms(OtMaterial& material);
	void submitTerrainUniforms(OtTerrain& terrain);
	void submitGrassUniforms(OtGrass& grass);
	void submitTextureSampler(OtSampler& sampler, int unit, OtAsset<OtTextureAsset>& texture);

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
	bool hasGrassEntities;
	bool hasParticlesEntities;
	bool renderingShadow;

	// directional light information
	glm::vec3 directionalLightDirection;
	glm::vec3 directionalLightColor;
	float directionalLightAmbient;
	bool renderDirectionalLight;
	bool castShadow;

	// key entities
	OtEntity iblEntity = OtEntityNull;
	OtEntity waterEntity = OtEntityNull;

	// uniforms
	OtUniformVec4 clipUniforms{"u_clip", 1};
	OtUniformVec4 lightingUniforms{"u_lighting", 4};
	OtUniformVec4 shadowUniforms{"u_shadow", 2};
	OtUniformVec4 albedoUniforms{"u_albedo", 2};
	OtUniformVec4 materialUniforms{"u_material", 5};
	OtUniformVec4 terrainUniforms{"u_terrain", 9};
	OtUniformVec4 grassUniforms{"u_grass", 6};
	OtUniformVec4 waterUniforms{"u_water", 4};
	OtUniformVec4 skyUniforms{"u_sky", 3};

	OtUniformMat4 viewUniform{"u_viewUniform", 1};
	OtUniformMat4 invViewProjUniform{"u_invViewProjUniform", 1};
	OtUniformMat4 shadowViewProjUniform{"u_shadowViewProjTransform", 4};

	OtUniformVec4 gridUniforms{"u_grid", 1};
	OtUniformVec4 highlightOutlineUniforms{"u_outline", 1};
	OtUniformVec4 pickingUniforms{"u_picking", 1};

	// samplers
	OtSampler iblBrdfLutSampler{"s_iblBrdfLut", OtSampler::linearSampling | OtSampler::repeatSampling};
	OtSampler iblIrradianceMapSampler{"s_iblIrradianceMap", OtSampler::linearSampling | OtSampler::repeatSampling};
	OtSampler iblEnvironmentMapSampler{"s_iblEnvironmentMap", OtSampler::linearSampling | OtSampler::repeatSampling};

	OtSampler albedoSampler{"s_albedoTexture", OtSampler::linearSampling | OtSampler::repeatSampling};
	OtSampler normalSampler{"s_normalTexture", OtSampler::linearSampling | OtSampler::repeatSampling};
	OtSampler metallicRoughnessSampler{"s_metallicRoughnessTexture", OtSampler::linearSampling | OtSampler::repeatSampling};
	OtSampler emissiveSampler{"s_emissiveTexture", OtSampler::linearSampling | OtSampler::repeatSampling};
	OtSampler aoSampler{"s_aoTexture", OtSampler::linearSampling | OtSampler::repeatSampling};

	OtSampler shadowMap0Sampler{"s_shadowMap0", OtSampler::pointSampling | OtSampler::clampSampling};
	OtSampler shadowMap1Sampler{"s_shadowMap1", OtSampler::pointSampling | OtSampler::clampSampling};
	OtSampler shadowMap2Sampler{"s_shadowMap2", OtSampler::pointSampling | OtSampler::clampSampling};
	OtSampler shadowMap3Sampler{"s_shadowMap3", OtSampler::pointSampling | OtSampler::clampSampling};

	OtSampler region1Sampler{"s_region1Texture", OtSampler::linearSampling | OtSampler::repeatSampling};
	OtSampler region2Sampler{"s_region2Texture", OtSampler::linearSampling | OtSampler::repeatSampling};
	OtSampler region3Sampler{"s_region3Texture", OtSampler::linearSampling | OtSampler::repeatSampling};
	OtSampler region4Sampler{"s_region4Texture", OtSampler::linearSampling | OtSampler::repeatSampling};
	OtSampler normalmapSampler{"s_normalMapTexture", OtSampler::linearSampling | OtSampler::repeatSampling};

	OtSampler lightingAlbedoSampler{"s_lightingAlbedoTexture", OtSampler::pointSampling | OtSampler::clampSampling};
	OtSampler lightingNormalSampler{"s_lightingNormalTexture", OtSampler::pointSampling | OtSampler::clampSampling};
	OtSampler lightingPbrSampler{"s_lightingPbrTexture", OtSampler::pointSampling | OtSampler::clampSampling};
	OtSampler lightingEmissiveSampler{"s_lightingEmissiveTexture", OtSampler::pointSampling | OtSampler::clampSampling};
	OtSampler lightingDepthSampler{"s_lightingDepthTexture", OtSampler::pointSampling | OtSampler::clampSampling};

	OtSampler skySampler{"s_skyTexture", OtSampler::linearSampling | OtSampler::repeatSampling};

	OtSampler waterNormalmapSampler{"s_waterNormalMapTexture", OtSampler::linearSampling | OtSampler::repeatSampling};
	OtSampler reflectionSampler{"s_reflectionTexture", OtSampler::linearSampling | OtSampler::repeatSampling};
	OtSampler refractionSampler{"s_refractionTexture", OtSampler::linearSampling | OtSampler::repeatSampling};
	OtSampler refractionDepthSampler{"s_refractionDepthTexture", OtSampler::linearSampling | OtSampler::repeatSampling};

	OtSampler selectedSampler{"s_selectedTexture", OtSampler::pointSampling | OtSampler::clampSampling};
};
