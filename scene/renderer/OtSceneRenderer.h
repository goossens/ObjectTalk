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
#include "OtFrameBuffer.h"
#include "OtGbuffer.h"
#include "OtNormalMapper.h"
#include "OtPass.h"
#include "OtSampler.h"
#include "OtShaderProgram.h"
#include "OtSphereGeometry.h"
#include "OtTileableFbm.h"
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
	// render passes
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
	void renderSkySphere(OtSceneRendererContext& ctx, OtPass& pass, OtSkySphereComponent& component);
	void renderForwardWater(OtSceneRendererContext& ctx, OtPass& pass, OtWaterComponent& water);
	void renderForwardGeometry(OtSceneRendererContext& ctx, OtPass& pass, OtEntity entity, OtGeometryComponent& geometry, OtMaterialComponent& material);
	void renderHighlight(OtSceneRendererContext& ctx, OtPass& pass, OtEntity entity);
	void renderBloom(OtSceneRendererContext& ctx, float bloomIntensity);

	// rendering tools
	void submitTextureSampler(OtSampler& sampler, int unit, OtAsset<OtTextureAsset>& texture);
	void submitMaterialUniforms(OtMaterial material);
	void submitPbrUniforms(OtPbrMaterial material);
	void submitLightUniforms(OtScene* scene, glm::vec3 cameraPosition);
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
	OtFrameBuffer deferredCompositeBuffer{OtFrameBuffer::rgbaFloat16Texture, OtFrameBuffer::dFloatTexture};
	OtFrameBuffer postProcessBuffer{OtFrameBuffer::rgba8Texture, OtFrameBuffer::noTexture};
	OtFrameBuffer selectedBuffer{OtFrameBuffer::r8Texture, OtFrameBuffer::noTexture};

	OtGbuffer reflectionRenderingBuffer;
	OtFrameBuffer reflectionCompositeBuffer{OtFrameBuffer::rgba16Texture, OtFrameBuffer::dFloatTexture};
	OtFrameBuffer refractionCompositeBuffer{OtFrameBuffer::rgba16Texture, OtFrameBuffer::dFloatTexture};
	OtFrameBuffer reflectionBuffer{OtFrameBuffer::rgba8Texture, OtFrameBuffer::noTexture};
	OtFrameBuffer refractionBuffer{OtFrameBuffer::rgba8Texture, OtFrameBuffer::noTexture};

	static constexpr int bloomDepth = 5;
	OtFrameBuffer bloomBuffer[bloomDepth];

	// standard geometries
	OtBoxGeometry unityBoxGeometry;
	OtSphereGeometry unitySphereGeometry;

	// generators/filters
	OtTileableFbm tileableFbm;
	OtNormalMapper normalMapper;

	// uniforms
	OtUniformVec4 pbrMaterialUniforms{"u_pbrMaterial", 5};
	OtUniformVec4 clipUniforms{"u_clip", 1};
	OtUniformVec4 terrainUniforms{"u_terrain", 9};
	OtUniformVec4 waterUniforms{"u_water", 4};
	OtUniformVec4 lightingUniforms{"u_lighting", 3};
	OtUniformVec4 skyUniforms{"u_sky", 3};
	OtUniformVec4 gridUniforms{"u_grid", 1};
	OtUniformVec4 outlineUniforms{"u_outline", 1};
	OtUniformVec4 bloomUniforms{"u_bloom", 1};
	OtUniformVec4 postProcessUniforms{"u_postProcess", 1};

	// samplers
	OtSampler deferredGeometryAlbedoSampler{"s_deferredGeometryAlbedoTexture"};
	OtSampler deferredGeometryNormalSampler{"s_deferredGeometryNormalTexture"};
	OtSampler deferredGeometryMetallicRoughnessSampler{"s_deferredGeometryMetallicRoughnessTexture"};
	OtSampler deferredGeometryEmissiveSampler{"s_deferredGeometryEmissiveSampler"};
	OtSampler deferredGeometryAoSampler{"s_deferredGeometryAoTexture"};

	OtSampler deferredLightingAlbedoSampler{"s_deferredLightingAlbedoTexture", OtSampler::pointSampling | OtSampler::clampSampling};
	OtSampler deferredLightingPositionSampler{"s_deferredLightingPositionTexture", OtSampler::pointSampling | OtSampler::clampSampling};
	OtSampler deferredLightingNormalSampler{"s_deferredLightingNormalTexture", OtSampler::pointSampling | OtSampler::clampSampling};
	OtSampler deferredLightingPbrSampler{"s_deferredLightingPbrTexture", OtSampler::pointSampling | OtSampler::clampSampling};
	OtSampler deferredLightingEmissiveSampler{"s_deferredLightingEmissiveTexture", OtSampler::pointSampling | OtSampler::clampSampling};
	OtSampler deferredLightingDepthSampler{"s_deferredLightingDepthTexture", OtSampler::pointSampling | OtSampler::clampSampling};

	OtSampler region1Sampler{"s_region1Sampler"};
	OtSampler region2Sampler{"s_region2Sampler"};
	OtSampler region3Sampler{"s_region3Sampler"};
	OtSampler region4Sampler{"s_region4Sampler"};

	OtSampler normalmapSampler{"s_normalmapSampler"};
	OtSampler reflectionSampler{"s_reflectionSampler"};
	OtSampler refractionSampler{"s_refractionSampler"};
	OtSampler refractionDepthSampler{"s_refractionDepthSampler"};

	OtSampler skySampler{"s_skySampler"};

	OtSampler selectedSampler{"s_selectedTexture", OtSampler::pointSampling | OtSampler::clampSampling};

	OtSampler postProcessSampler{"s_postProcessTexture", OtSampler::pointSampling | OtSampler::clampSampling};
	OtSampler bloomSampler{"s_bloomTexture", OtSampler::pointSampling | OtSampler::clampSampling};

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
	OtShaderProgram skySphereProgram{"OtSkyVS", "OtSkySphereFS"};
	OtShaderProgram bloomDownSampleProgram{"OtFilterVS", "OtBloomDownSampleFS"};
	OtShaderProgram bloomUpSampleProgram{"OtFilterVS", "OtBloomUpSampleFS"};
	OtShaderProgram postProcessProgram{"OtFilterVS", "OtPostProcessFS"};
};
