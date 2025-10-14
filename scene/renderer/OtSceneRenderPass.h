//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtSampler.h"
#include "OtShaderProgram.h"
#include "OtTextureAsset.h"
#include "OtUniformMat4.h"
#include "OtUniformVec4.h"

#include "OtGrassComponent.h"
#include "OtMaterial.h"
#include "OtSceneRendererContext.h"
#include "OtTerrain.h"


//
//	OtSceneRenderPass
//

class OtSceneRenderPass {
protected:
	// utility methods for subclasses
	void submitClippingUniforms(OtSceneRendererContext& ctx);
	void submitLightingUniforms(OtSceneRendererContext& ctx);
	void submitShadowUniforms(OtSceneRendererContext& ctx);
	void submitAlbedoUniforms(OtMaterial& material);
	void submitMaterialUniforms(OtMaterial& material);
	void submitTerrainUniforms(OtTerrain& terrain);
	void submitGrassUniforms(OtGrassComponent* grass);
	void submitTextureSampler(OtSampler& sampler, int unit, OtAsset<OtTextureAsset>& texture);

private:
	// private properties
	OtUniformVec4 clipUniforms{"u_clip", 1};
	OtUniformVec4 lightingUniforms{"u_lighting", 4};
	OtUniformVec4 shadowUniforms{"u_shadow", 2};
	OtUniformVec4 albedoUniforms{"u_albedo", 2};
	OtUniformVec4 materialUniforms{"u_material", 5};
	OtUniformVec4 terrainUniforms{"u_terrain", 9};
	OtUniformVec4 grassUniforms{"u_grass", 6};

	OtUniformMat4 viewUniform{"u_viewUniform", 1};
	OtUniformMat4 shadowViewProjUniform{"u_shadowViewProjTransform", 4};

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
};
