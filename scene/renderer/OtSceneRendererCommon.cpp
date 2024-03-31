
//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>
#include <cstdint>
#include <chrono>

#include "OtException.h"

#include "OtSceneRenderer.h"


//
//	OtSceneRenderer::getRunningTime
//

float OtSceneRenderer::getRunningTime() {
	static uint64_t start = 0;

	using namespace std::chrono;
	uint64_t time = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

	if (start == 0) {
		start = time;
	}

	return float(double(time - start) / 1000.0);
}


//
//	OtSceneRenderer::getTextureAssetWidth
//

size_t OtSceneRenderer::getTextureAssetWidth(OtAsset<OtTextureAsset>& texture) {
	return 	texture.isReady() ? texture->getTexture().getWidth() : 1;
}


//
//	OtSceneRenderer::getTextureAssetHeight
//

size_t OtSceneRenderer::getTextureAssetHeight(OtAsset<OtTextureAsset>& texture) {
	return 	texture.isReady() ? texture->getTexture().getHeight() : 1;
}


//
//	OtSceneRenderer::submitTextureSampler
//

void OtSceneRenderer::submitTextureSampler(OtSampler& sampler, int unit, OtAsset<OtTextureAsset>& texture) {
	if (texture.isReady()) {
		sampler.submit(unit, texture->getTexture());

	} else {
		sampler.submitDummyTexture(unit);
	}
}


//
//	OtSceneRenderer::submitCubeMapSampler
//

void OtSceneRenderer::submitCubeMapSampler(OtSampler& sampler, int unit, OtAsset<OtCubeMapAsset>& cubemap) {
	if (cubemap.isReady()) {
		sampler.submit(unit, cubemap->getCubeMap());

	} else {
		sampler.submitDummyCubeMap(unit);
	}

}


//
//	OtSceneRenderer::submitMaterialUniforms
//

void OtSceneRenderer::submitMaterialUniforms(OtMaterial& material) {
	// set the uniform values
	glm::vec4* uniforms = pbrMaterialUniforms.getValues();
	uniforms[0] = material.albedo;

	uniforms[1] = glm::vec4(
		material.metallic,
		material.roughness,
		material.ao,
		material.scale);

	uniforms[2] = glm::vec4(
		material.emissive,
		0.0f);

	uniforms[3] = glm::vec4(
		material.albedoTexture.isReady(),
		material.metallicRoughnessTexture.isReady(),
		0.0f,
		0.0f);

	uniforms[4] = glm::vec4(
		material.emissiveTexture.isReady(),
		material.aoTexture.isReady(),
		material.normalTexture.isReady(),
		0.0);

	// submit the uniforms
	pbrMaterialUniforms.submit();

	// submit all material textures (or dummies if they are not set (yet))
	submitTextureSampler(albedoSampler, 0, material.albedoTexture);
	submitTextureSampler(metallicRoughnessSampler, 1, material.metallicRoughnessTexture);
	submitTextureSampler(emissiveSampler, 2, material.emissiveTexture);
	submitTextureSampler(aoSampler, 3, material.aoTexture);
	submitTextureSampler(normalSampler, 4, material.normalTexture);
}


//
//	OtSceneRenderer::submitTerrainUniforms
//

void OtSceneRenderer::submitTerrainUniforms(OtTerrain terrain) {
	// get access to key terrain information
	OtTerrainHeights& heights = terrain->heights;
	OtTerrainMaterial& material = terrain->material;

	// set the uniform values
	glm::vec4* uniforms = terrainUniforms.getValues();

	uniforms[0] = glm::vec4(
		terrain->hScale,
		terrain->vScale,
		terrain->vOffset,
		float(heights.heightmapSize));

	uniforms[1] = glm::vec4(
		getTextureAssetWidth(material.region1Texture),
		getTextureAssetWidth(material.region2Texture),
		getTextureAssetWidth(material.region3Texture),
		getTextureAssetWidth(material.region4Texture));

	uniforms[2] = glm::vec4(
		material.region1TextureScale,
		material.region2TextureScale,
		material.region3TextureScale,
		material.region4TextureScale);

	uniforms[3] = glm::vec4(
		material.region1Transition,
		material.region2Transition,
		material.region3Transition,
		0.0f);

	uniforms[4] = glm::vec4(
		material.region1Overlap,
		material.region2Overlap,
		material.region3Overlap,
		0.0f);

	uniforms[5] = glm::vec4(material.region1Color, material.region1Texture.isReady());
	uniforms[6] = glm::vec4(material.region2Color, material.region2Texture.isReady());
	uniforms[7] = glm::vec4(material.region3Color, material.region3Texture.isReady());
	uniforms[8] = glm::vec4(material.region4Color, material.region4Texture.isReady());

	// submit the uniforms
	terrainUniforms.submit();

	// bind the normalmap texture (which includes the heightmap in the alpha/w component)
	heights.normalmap.bindColorTexture(normalmapSampler, 0);

	// submit all material textures (or dummies if they are not set)
	submitTextureSampler(region1Sampler, 1, material.region1Texture);
	submitTextureSampler(region2Sampler, 2, material.region2Texture);
	submitTextureSampler(region3Sampler, 3, material.region3Texture);
	submitTextureSampler(region4Sampler, 4, material.region4Texture);
}


//
//	OtSceneRenderer::submitLightingUniforms
//

void OtSceneRenderer::submitLightingUniforms(OtSceneRendererContext& ctx) {
	// build and submit the light uniforms
	glm::vec4* uniforms = lightingUniforms.getValues();
	uniforms[0] = glm::vec4(ctx.camera.cameraPosition, float(ctx.hasDirectionalLighting));
	uniforms[1] = glm::vec4(ctx.directionalLightDirection, 0.0f);
	uniforms[2] = glm::vec4(ctx.directionalLightColor, ctx.directionalLightAmbient);
	lightingUniforms.submit();

	// build and submit the IBL uniform
	iblUniform.setValue(0, float(ctx.hasImageBasedLighting), float(maxEnvLevel), 0.0f, 0.0f);
	iblUniform.submit();

	// submit the IBL samplers
	iblBrdfLutSampler.submit(5, iblBrdfLut);
	iblIrradianceMapSampler.submit(6, iblIrradianceMap);
	iblEnvironmentMapSampler.submit(7, iblEnvironmentMap);

	// build and submit the shadow uniforms
	shadowUniforms.setValue(0, float(ctx.castShadow), 1.0f / shadowMapSize, 0.0f, 0.0f);
	shadowUniforms.setValue(1, csm.getDistance(0), csm.getDistance(1), csm.getDistance(2), csm.getDistance(3)),
	shadowUniforms.submit();

	shadowViewProjUniform.setValue(0, csm.getViewProjectionMatrix(0));
	shadowViewProjUniform.setValue(1, csm.getViewProjectionMatrix(1));
	shadowViewProjUniform.setValue(2, csm.getViewProjectionMatrix(2));
	shadowViewProjUniform.setValue(3, csm.getViewProjectionMatrix(3));
	shadowViewProjUniform.submit();

	viewUniform.set(0, ctx.camera.viewMatrix);
	viewUniform.submit();

	// submit the shadow samplers
	csm.getFrameBuffer(0).bindDepthTexture(shadowMap0Sampler, 10);
	csm.getFrameBuffer(1).bindDepthTexture(shadowMap1Sampler, 11);
	csm.getFrameBuffer(2).bindDepthTexture(shadowMap2Sampler, 12);
	csm.getFrameBuffer(3).bindDepthTexture(shadowMap3Sampler, 13);
}


//
//	OtSceneRenderer::submitClippingUniforms
//

void OtSceneRenderer::submitClippingUniforms(OtSceneRendererContext& ctx) {
	clipUniforms.setValue(0, ctx.clippingPlane);
	clipUniforms.submit();
}
