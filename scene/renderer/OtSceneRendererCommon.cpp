
//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>

#include "OtException.h"

#include "OtMaterials.h"
#include "OtSceneRenderer.h"


//
//	OtSceneRenderer::submitMaterialUniforms
//

void OtSceneRenderer::submitMaterialUniforms(OtMaterial material) {
	// submit based on type
	if (material.isKindOf<OtPbrMaterialClass>()) {
		submitPbrUniforms(OtPbrMaterial(material));

	} else {
		OtError("Internal error: invalid material type [%s]", material->getTypeName());
	}
}


//
//	OtSceneRenderer::submitSampler
//

void OtSceneRenderer::submitSampler(OtSampler& sampler, int unit, OtAsset<OtTextureAsset>& texture) {
	if (texture.isReady()) {
		sampler.submit(unit, texture->getTexture());

	} else {
		sampler.submit(unit);
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
	submitSampler(deferredGeometryAlbedoSampler, 0, material->albedoTexture);
	submitSampler(deferredGeometryMetallicRoughnessSampler, 1, material->metallicRoughnessTexture);
	submitSampler(deferredGeometryEmissiveSampler, 2, material->emissiveTexture);
	submitSampler(deferredGeometryAoSampler, 3, material->aoTexture);
	submitSampler(deferredGeometryNormalSampler, 4, material->normalTexture);
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
	submitSampler(region1Sampler, 1, material.region1Texture);
	submitSampler(region2Sampler, 2, material.region2Texture);
	submitSampler(region3Sampler, 3, material.region3Texture);
	submitSampler(region4Sampler, 4, material.region4Texture);
}


//
//	OtSceneRenderer::submitLightUniforms
//

void OtSceneRenderer::submitLightUniforms(OtScene* scene) {
	// get the directional light information
	glm::vec3 direction = glm::vec3(0.0f);
	glm::vec3 color = glm::vec3(0.0f);
	float ambient = 0.0f;

	for (auto&& [entity, light, transform] : scene->view<OtDirectionalLightComponent, OtTransformComponent>().each()) {
		direction = transform.getTransform()[3];
		color = light.color;
		ambient = light.ambient;
	}

	for (auto&& [entity, sky] : scene->view<OtSkyComponent>().each()) {
		direction = sky.getDirectionToSun();
		color = glm::vec3(0.2f + std::clamp(sky.elevation / 10.0f, 0.0f, 0.8f));
		ambient = std::clamp((sky.elevation + 6.0f) / 200.0f, 0.0f, 0.2f);
	}

	// build and submit the light uniforms
	glm::vec4* uniforms = lightingUniforms.getValues();
	uniforms[0] = glm::vec4(cameraPosition, 0.0f);
	uniforms[1] = glm::vec4(direction, 0.0f);
	uniforms[2] = glm::vec4(color, ambient);
	lightingUniforms.submit();
}
