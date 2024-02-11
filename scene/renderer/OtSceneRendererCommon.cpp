
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

#include "OtMaterials.h"
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
//	OtSceneRenderer::submitMaterialUniforms
//

void OtSceneRenderer::submitMaterialUniforms(OtMaterial material) {
	// submit based on type
	if (material.isKindOf<OtPbrMaterialClass>()) {
		submitPbrUniforms(OtPbrMaterial(material));

	} else {
		OtError("Internal error: invalid material type [{}]", material->getTypeName());
	}
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
//	OtSceneRenderer::submitPbrUniforms
//

void OtSceneRenderer::submitPbrUniforms(OtPbrMaterial material) {
	// set the uniform values
	glm::vec4* uniforms = pbrMaterialUniforms.getValues();
	uniforms[0] = material->albedo;

	uniforms[1] = glm::vec4(
		material->metallic,
		material->roughness,
		material->ao,
		material->scale);

	uniforms[2] = glm::vec4(
		material->emissive,
		0.0f);

	uniforms[3] = glm::vec4(
		material->albedoTexture.isReady(),
		material->metallicRoughnessTexture.isReady(),
		0.0f,
		0.0f);

	uniforms[4] = glm::vec4(
		material->emissiveTexture.isReady(),
		material->aoTexture.isReady(),
		material->normalTexture.isReady(),
		0.0);

	// submit the uniforms
	pbrMaterialUniforms.submit();

	// submit all material textures (or dummies if they are not set (yet))
	submitTextureSampler(albedoSampler, 0, material->albedoTexture);
	submitTextureSampler(metallicRoughnessSampler, 1, material->metallicRoughnessTexture);
	submitTextureSampler(emissiveSampler, 2, material->emissiveTexture);
	submitTextureSampler(aoSampler, 3, material->aoTexture);
	submitTextureSampler(normalSampler, 4, material->normalTexture);
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
//	OtSceneRenderer::submitLightUniforms
//

void OtSceneRenderer::submitLightUniforms(OtSceneRendererContext& ctx) {
	glm::vec3 direction = glm::vec3(0.0f);
	glm::vec3 color = glm::vec3(0.0f);
	float ambient = 0.0f;

	// get the directional light information (if required)
	if (ctx.hasDirectionalLighting) {
		for (auto&& [entity, light, transform] : ctx.scene->view<OtDirectionalLightComponent, OtTransformComponent>().each()) {
			direction = transform.getTransform()[3];
			color = light.color;
			ambient = light.ambient;
		}

		for (auto&& [entity, sky] : ctx.scene->view<OtSkyComponent>().each()) {
			direction = sky.getDirectionToSun();
			color = glm::vec3(0.2f + std::clamp(sky.elevation / 10.0f, 0.0f, 0.8f));
			ambient = std::clamp((sky.elevation + 6.0f) / 200.0f, 0.0f, 0.2f);
		}
	}

	// build and submit the light uniforms
	glm::vec4* uniforms = lightingUniforms.getValues();
	uniforms[0] = glm::vec4(ctx.camera.cameraPosition, float(ctx.hasDirectionalLighting));
	uniforms[1] = glm::vec4(direction, 0.0f);
	uniforms[2] = glm::vec4(color, ambient);
	lightingUniforms.submit();

	// build and submit the IBL uniform
	iblUniform.setValue(0, float(ctx.hasImageBasedLighting), float(maxEnvLevel), 0.0f, 0.0f);
	iblUniform.submit();

	// submit the IBL samplers
	iblBrdfLutSampler.submit(5, iblBrdfLut);
	iblIrradianceMapSampler.submit(6, iblIrradianceMap);
	iblEnvironmentMapSampler.submit(7, iblEnvironmentMap);
}


//
//	OtSceneRenderer::submitClippingUniforms
//

void OtSceneRenderer::submitClippingUniforms(const glm::vec4& clippingPlane) {
	clipUniforms.setValue(0, clippingPlane);
	clipUniforms.submit();
}
