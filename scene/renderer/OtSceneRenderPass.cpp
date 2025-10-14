//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtSceneRenderPass.h"


//
//	OtSceneRenderPass::submitClippingUniforms
//

void OtSceneRenderPass::submitClippingUniforms(OtSceneRendererContext& ctx) {
	clipUniforms.setValue(0, ctx.clippingPlane);
	clipUniforms.submit();
}


//
//	OtSceneRenderPass::submitLightingUniforms
//

void OtSceneRenderPass::submitLightingUniforms(OtSceneRendererContext& ctx) {
	// build and submit the light uniforms
	glm::vec4* uniforms = lightingUniforms.getValues();
	uniforms[0] = glm::vec4(ctx.camera.position, float(ctx.hasDirectionalLighting));
	uniforms[1] = glm::vec4(ctx.directionalLightDirection, 0.0f);
	uniforms[2] = glm::vec4(ctx.directionalLightColor, ctx.directionalLightAmbient);
	uniforms[3] = glm::vec4(float(ctx.hasImageBasedLighting), float(ctx.hasImageBasedLighting ? ctx.ibl->maxEnvLevel : 0), 0.0f, 0.0f);
	lightingUniforms.submit();

	// submit the IBL samplers
	if (ctx.hasImageBasedLighting) {
		iblBrdfLutSampler.submit(5, ctx.ibl->iblBrdfLut);
		iblIrradianceMapSampler.submit(6, ctx.ibl->iblIrradianceMap);
		iblEnvironmentMapSampler.submit(7, ctx.ibl->iblEnvironmentMap);

	} else {
		iblBrdfLutSampler.submitDummyTexture(5);
		iblIrradianceMapSampler.submitDummyCubeMap(6);
		iblEnvironmentMapSampler.submitDummyCubeMap(7);
	}
}


//
//	OtSceneRenderPass::submitShadowUniforms
//

void OtSceneRenderPass::submitShadowUniforms(OtSceneRendererContext& ctx) {
	// build and submit the shadow uniforms
	shadowUniforms.setValue(0, float(ctx.castShadow), 1.0f / ctx.csm->getSize(), 0.0f, 0.0f);
	shadowUniforms.setValue(1, ctx.csm->getDistance(0), ctx.csm->getDistance(1), ctx.csm->getDistance(2), ctx.csm->getDistance(3)),
	shadowUniforms.submit();

	shadowViewProjUniform.setValue(0, ctx.csm->getCamera(0).viewProjectionMatrix);
	shadowViewProjUniform.setValue(1, ctx.csm->getCamera(1).viewProjectionMatrix);
	shadowViewProjUniform.setValue(2, ctx.csm->getCamera(2).viewProjectionMatrix);
	shadowViewProjUniform.setValue(3, ctx.csm->getCamera(3).viewProjectionMatrix);
	shadowViewProjUniform.submit();

	viewUniform.set(0, ctx.camera.viewMatrix);
	viewUniform.submit();

	// submit the shadow samplers
	ctx.csm->getFrameBuffer(0).bindDepthTexture(shadowMap0Sampler, 8);
	ctx.csm->getFrameBuffer(1).bindDepthTexture(shadowMap1Sampler, 9);
	ctx.csm->getFrameBuffer(2).bindDepthTexture(shadowMap2Sampler, 10);
	ctx.csm->getFrameBuffer(3).bindDepthTexture(shadowMap3Sampler, 11);
}


//
//	OtSceneRenderPass::submitAlbedoUniforms
//

void OtSceneRenderPass::submitAlbedoUniforms(OtMaterial& material) {
	// set the uniform values
	albedoUniforms.setValue(0, material.albedo);
	albedoUniforms.setValue(1, material.albedoTexture.isReady(), material.scale, 0.0f, 0.0f);
	albedoUniforms.submit();

	// submit albedo texture (or dummy if it isn't set (yet))
	submitTextureSampler(albedoSampler, 0, material.albedoTexture);
}


//
//	OtSceneRenderPass::submitMaterialUniforms
//

void OtSceneRenderPass::submitMaterialUniforms(OtMaterial& material) {
	// set the uniform values
	glm::vec4* uniforms = materialUniforms.getValues();
	uniforms[0] = material.albedo;

	uniforms[1] = glm::vec4(
		material.metallic,
		material.roughness,
		material.ao,
		0.0f);

	uniforms[2] = glm::vec4(
		material.emissive,
		0.0f);

	uniforms[3] = glm::vec4(
		material.offset,
		material.scale,
		material.albedoTexture.isReady());

	uniforms[4] = glm::vec4(
		material.metallicRoughnessTexture.isReady(),
		material.emissiveTexture.isReady(),
		material.aoTexture.isReady(),
		material.normalTexture.isReady());

	// submit the uniforms
	materialUniforms.submit();

	// submit all material textures (or dummies if they are not set (yet))
	submitTextureSampler(albedoSampler, 0, material.albedoTexture);
	submitTextureSampler(metallicRoughnessSampler, 1, material.metallicRoughnessTexture);
	submitTextureSampler(emissiveSampler, 2, material.emissiveTexture);
	submitTextureSampler(aoSampler, 3, material.aoTexture);
	submitTextureSampler(normalSampler, 4, material.normalTexture);
}


//
//	OtSceneRenderPass::submitTerrainUniforms
//

void OtSceneRenderPass::submitTerrainUniforms(OtTerrain& terrain) {
	// get access to key terrain information
	OtTerrainHeights& heights = terrain.heights;
	OtTerrainMaterial& material = terrain.material;

	// set the uniform values
	glm::vec4* uniforms = terrainUniforms.getValues();

	uniforms[0] = glm::vec4(
		terrain.hScale,
		terrain.vScale,
		terrain.vOffset,
		float(heights.heightmapSize));

	uniforms[1] = glm::vec4(
		material.region1Texture.isReady() ? material.region1Texture->getTexture().getWidth() : 1,
		material.region2Texture.isReady() ? material.region2Texture->getTexture().getWidth() : 1,
		material.region3Texture.isReady() ? material.region3Texture->getTexture().getWidth() : 1,
		material.region4Texture.isReady() ? material.region4Texture->getTexture().getWidth() : 1);

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
//	OtSceneRenderPass::submitGrassUniforms
//

void OtSceneRenderPass::submitGrassUniforms(OtGrassComponent* grass) {
	grassUniforms.setValue(0, grass->patchWidth, grass->patchDepth, static_cast<float>(grass->bladeSegments), static_cast<float>(grass->blades));
	grassUniforms.setValue(1, grass->bladeWidth, grass->bladeHeight, grass->bladePointiness, grass->bladeCurve);
	grassUniforms.setValue(2, static_cast<float>(ImGui::GetTime()), glm::radians(grass->windDirection), grass->windStrength, 0.0f);
	grassUniforms.setValue(3, grass->widthVariation, grass->heightVariation, grass->windVariation, grass->colorVariation);
	grassUniforms.setValue(4, grass->baseColor, 0.0f);
	grassUniforms.setValue(5, grass->tipColor, 0.0f);
	grassUniforms.submit();
}


//
//	OtSceneRenderPass::submitTextureSampler
//

void OtSceneRenderPass::submitTextureSampler(OtSampler& sampler, int unit, OtAsset<OtTextureAsset>& texture) {
	if (texture.isReady()) {
		sampler.submit(unit, texture->getTexture());

	} else {
		sampler.submitDummyTexture(unit);
	}
}
