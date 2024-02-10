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

#include "OtBoxGeometry.h"
#include "OtComputeProgram.h"
#include "OtCubeMap.h"
#include "OtFrameBuffer.h"
#include "OtGbuffer.h"
#include "OtNormalMapper.h"
#include "OtPass.h"
#include "OtSampler.h"
#include "OtShaderProgram.h"
#include "OtSphereGeometry.h"
#include "OtTexture.h"
#include "OtTileableFbm.h"
#include "OtUniformMat4.h"
#include "OtUniformVec4.h"

#include "OtCamera.h"
#include "OtEntity.h"
#include "OtScene.h"
#include "OtSceneRendererContext.h"


//
//	OtSceneRenderer
//

class OtSceneRenderer {
public:
	// set properties
	void setGridScale(float gs) { gridScale = gs; }

	// render specified scene
	int render(OtCamera& camera, OtScene* scene, OtEntity selected=OtEntityNull);

private:
	// give the debugger access to the inner circle
	friend class OtSceneRendererDebug;

	// render passes
	void renderIblPass(OtSceneRendererContext& ctx);
	void renderReflectionPass(OtSceneRendererContext& ctx);
	// void renderShadowPass(OtSceneRendererContext& ctx);
	void renderDeferredGeometryPass(OtSceneRendererContext& ctx);
	void renderBackgroundPass(OtSceneRendererContext& ctx);
	void renderSkyPass(OtSceneRendererContext& ctx);
	void renderDeferredLightingPass(OtSceneRendererContext& ctx);
	void renderForwardGeometryPass(OtSceneRendererContext& ctx);
	void renderGridPass(OtSceneRendererContext& ctx);
	void renderHighlightPass(OtSceneRendererContext& ctx, OtEntity selected);
	void renderPostProcessingPass(OtSceneRendererContext& ctx, bool renderPostProcessingEffects=true);

	// render entities
	void renderReflectionRefractionScene(OtSceneRendererContext& ctx);
	void renderDeferredGeometry(OtSceneRendererContext& ctx, OtPass& pass, OtEntity entity, OtGeometryComponent& geometry, OtMaterialComponent& material);
	void renderDeferredModel(OtSceneRendererContext& ctx, OtPass& pass, OtEntity entity, OtModelComponent& model);
	void renderDeferredTerrain(OtSceneRendererContext& ctx, OtPass& pass, OtTerrainComponent& terrain);
	void renderSky(OtSceneRendererContext& ctx, OtPass& pass, OtSkyComponent& component);
	void renderSkyBox(OtSceneRendererContext& ctx, OtPass& pass, OtSkyBoxComponent& component);
	void renderForwardWater(OtSceneRendererContext& ctx, OtPass& pass, OtWaterComponent& water);
	void renderForwardGeometry(OtSceneRendererContext& ctx, OtPass& pass, OtEntity entity, OtGeometryComponent& geometry, OtMaterialComponent& material);
	void renderHighlight(OtSceneRendererContext& ctx, OtPass& pass, OtEntity entity);
	void renderBloom(OtSceneRendererContext& ctx, float bloomIntensity);

	// rendering tools
	void submitTextureSampler(OtSampler& sampler, int unit, OtAsset<OtTextureAsset>& texture);
	void submitCubeMapSampler(OtSampler& sampler, int unit, OtAsset<OtCubeMapAsset>& cubemap);
	void submitMaterialUniforms(OtMaterial material);
	void submitPbrUniforms(OtPbrMaterial material);
	void submitLightUniforms(OtSceneRendererContext& ctx);
	void submitTerrainUniforms(OtTerrain terrain);
	void submitClippingUniforms(const glm::vec4& clippingPlane);

	// helpers
	float getRunningTime();
	size_t getTextureAssetWidth(OtAsset<OtTextureAsset>& texture);
	size_t getTextureAssetHeight(OtAsset<OtTextureAsset>& texture);

	// grid scale (0.0 means no grid)
	float gridScale = 0.0f;

	// framebuffers
	OtGbuffer deferredRenderingBuffer;
	OtFrameBuffer deferredCompositeBuffer{OtTexture::rgbaFloat16Texture, OtTexture::dFloatTexture};
	OtFrameBuffer postProcessBuffer{OtTexture::rgba8Texture, OtTexture::noTexture};
	OtFrameBuffer selectedBuffer{OtTexture::r8Texture, OtTexture::noTexture};

	OtGbuffer reflectionRenderingBuffer;
	OtFrameBuffer reflectionCompositeBuffer{OtTexture::rgba16Texture, OtTexture::dFloatTexture};
	OtFrameBuffer refractionCompositeBuffer{OtTexture::rgba16Texture, OtTexture::dFloatTexture};
	OtFrameBuffer reflectionBuffer{OtTexture::rgba8Texture, OtTexture::noTexture};
	OtFrameBuffer refractionBuffer{OtTexture::rgba8Texture, OtTexture::noTexture};

	static constexpr int bloomDepth = 5;
	OtFrameBuffer bloomBuffer[bloomDepth];

	// standard geometries
	OtBoxGeometry unityBoxGeometry;
	OtSphereGeometry unitySphereGeometry;

	// generators/filters
	OtTileableFbm tileableFbm;
	OtNormalMapper normalMapper;

	// uniforms
	OtUniformVec4 iblEnviromentUniform{"u_iblEnvironment", 1};
	OtUniformVec4 pbrMaterialUniforms{"u_pbrMaterial", 5};
	OtUniformVec4 clipUniforms{"u_clip", 1};
	OtUniformVec4 terrainUniforms{"u_terrain", 9};
	OtUniformVec4 waterUniforms{"u_water", 4};
	OtUniformVec4 lightingUniforms{"u_lighting", 3};
	OtUniformVec4 iblUniform{"u_ibl", 1};
	OtUniformVec4 skyUniforms{"u_sky", 3};
	OtUniformVec4 gridUniforms{"u_grid", 1};
	OtUniformVec4 outlineUniforms{"u_outline", 1};
	OtUniformVec4 bloomUniforms{"u_bloom", 1};
	OtUniformVec4 postProcessUniforms{"u_postProcess", 1};

	OtUniformMat4 invViewProjUniform{"u_invViewProjUniform", 1};

	// textures
	OtTexture iblBrdfLut;

	// cubemaps
	OtCubeMap* iblSkyMap;
	OtCubeMap iblIrradianceMap;
	OtCubeMap iblEnvironmentMap;

	// image based lighting data
	int iblSkyMapVersion = 0;
	int maxEnvLevel;

	// samplers
	OtSampler cubemapSampler{"s_cubemap"};

	OtSampler albedoSampler{"s_albedoTexture"};
	OtSampler normalSampler{"s_normalTexture"};
	OtSampler metallicRoughnessSampler{"s_metallicRoughnessTexture"};
	OtSampler emissiveSampler{"s_emissiveTexture"};
	OtSampler aoSampler{"s_aoTexture"};

	OtSampler lightingAlbedoSampler{"s_lightingAlbedoTexture", OtTexture::pointSampling | OtTexture::clampSampling};
	OtSampler lightingNormalSampler{"s_lightingNormalTexture", OtTexture::pointSampling | OtTexture::clampSampling};
	OtSampler lightingPbrSampler{"s_lightingPbrTexture", OtTexture::pointSampling | OtTexture::clampSampling};
	OtSampler lightingEmissiveSampler{"s_lightingEmissiveTexture", OtTexture::pointSampling | OtTexture::clampSampling};
	OtSampler lightingDepthSampler{"s_lightingDepthTexture", OtTexture::pointSampling | OtTexture::clampSampling};

	OtSampler iblBrdfLutSampler{"s_iblBrdfLut"};
	OtSampler iblIrradianceMapSampler{"s_iblIrradianceMap"};
	OtSampler iblEnvironmentMapSampler{"s_iblEnvironmentMap"};

	OtSampler region1Sampler{"s_region1Texture"};
	OtSampler region2Sampler{"s_region2Texture"};
	OtSampler region3Sampler{"s_region3Texture"};
	OtSampler region4Sampler{"s_region4Texture"};

	OtSampler normalmapSampler{"s_normalmapTexture"};
	OtSampler reflectionSampler{"s_reflectionTexture"};
	OtSampler refractionSampler{"s_refractionTexture"};
	OtSampler refractionDepthSampler{"s_refractionDepthTexture"};

	OtSampler skySampler{"s_skyTexture"};

	OtSampler selectedSampler{"s_selectedTexture", OtTexture::pointSampling | OtTexture::clampSampling};

	OtSampler postProcessSampler{"s_postProcessTexture", OtTexture::pointSampling | OtTexture::clampSampling};
	OtSampler bloomSampler{"s_bloomTexture", OtTexture::pointSampling | OtTexture::clampSampling};

	// shader programs
	OtShaderProgram deferredPbrProgram{"OtDeferredVS", "OtDeferredPbrFS"};
	OtShaderProgram deferredInstancingProgram{"OtDeferredInstancingVS", "OtDeferredPbrFS"};
	OtShaderProgram deferredTerrainProgram{"OtTerrainVS", "OtTerrainFS"};
	OtShaderProgram deferredLightingProgram{"OtDeferredLightingVS", "OtDeferredLightingFS"};
	OtShaderProgram forwardPbrProgram{"OtForwardVS", "OtForwardPbrFS"};
	OtShaderProgram forwardInstancingProgram{"OtForwardInstancingVS", "OtForwardPbrFS"};
	OtShaderProgram forwardWaterProgram{"OtWaterVS", "OtWaterFS"};
	OtShaderProgram gridProgram{"OtGridVS", "OtGridFS"};
	OtShaderProgram selectProgram{"OtSelectVS", "OtSelectFS"};
	OtShaderProgram outlineProgram{"OtOutlineVS", "OtOutlineFS"};
	OtShaderProgram skyProgram{"OtSkyVS", "OtSkyFS"};
	OtShaderProgram skyBoxProgram{"OtSkyVS", "OtSkyBoxFS"};
	OtShaderProgram bloomDownSampleProgram{"OtFilterVS", "OtBloomDownSampleFS"};
	OtShaderProgram bloomUpSampleProgram{"OtFilterVS", "OtBloomUpSampleFS"};
	OtShaderProgram postProcessProgram{"OtFilterVS", "OtPostProcessFS"};

	// compute programs
	OtComputeProgram program{"OtBrdfLutCS"};
	OtComputeProgram irradianceProgram{"OtIblIrradianceMapCS"};
	OtComputeProgram envmapProgram{"OtIblEnvironmentMapCS"};
};
