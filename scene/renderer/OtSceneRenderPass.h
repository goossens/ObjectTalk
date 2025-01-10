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
	void submitTextureSampler(OtSampler& sampler, int unit, OtAsset<OtTextureAsset>& texture);

private:
	// private properties
	OtUniformVec4 clipUniforms{"u_clip", 1};
	OtUniformVec4 lightingUniforms{"u_lighting", 4};
	OtUniformVec4 shadowUniforms{"u_shadow", 2};
	OtUniformVec4 albedoUniforms{"u_albedo", 2};
	OtUniformVec4 materialUniforms{"u_material", 5};
	OtUniformVec4 terrainUniforms{"u_terrain", 9};

	OtUniformMat4 viewUniform{"u_viewUniform", 1};
	OtUniformMat4 shadowViewProjUniform{"u_shadowViewProjTransform", 4};

	OtSampler iblBrdfLutSampler{"s_iblBrdfLut"};
	OtSampler iblIrradianceMapSampler{"s_iblIrradianceMap"};
	OtSampler iblEnvironmentMapSampler{"s_iblEnvironmentMap"};

	OtSampler albedoSampler{"s_albedoTexture"};
	OtSampler normalSampler{"s_normalTexture"};
	OtSampler metallicRoughnessSampler{"s_metallicRoughnessTexture"};
	OtSampler emissiveSampler{"s_emissiveTexture"};
	OtSampler aoSampler{"s_aoTexture"};

	OtSampler shadowMap0Sampler{"s_shadowMap0"};
	OtSampler shadowMap1Sampler{"s_shadowMap1"};
	OtSampler shadowMap2Sampler{"s_shadowMap2"};
	OtSampler shadowMap3Sampler{"s_shadowMap3"};

	OtSampler region1Sampler{"s_region1Texture"};
	OtSampler region2Sampler{"s_region2Texture"};
	OtSampler region3Sampler{"s_region3Texture"};
	OtSampler region4Sampler{"s_region4Texture"};
	OtSampler normalmapSampler{"s_normalmapTexture"};
};
