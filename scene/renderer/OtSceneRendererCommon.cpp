
//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtException.h"

#include "OtMaterials.h"
#include "OtSceneRenderer.h"


//
//	OtSceneRenderer::submitMaterialUniforms
//

void OtSceneRenderer::submitMaterialUniforms(OtScene* scene, OtEntity entity) {
	// get the material component
	auto& material = scene->getComponent<OtMaterialComponent>(entity).material;

	// submit based on type
	if (material.isKindOf<OtPbrMaterialClass>()) {
		submitPbrUniforms(OtPbrMaterial(material));

	} else if (material.isKindOf<OtBlendMapMaterialClass>()) {
		submitBlendMapUniforms(OtBlendMapMaterial(material));

	} else {
		OtError("Internal error: unknown material type [%s]", material->getTypeName());
	}
}


//
//	submitSampler
//

static void submitSampler(OtSampler& sampler, int unit, OtAsset<OtTextureAsset>& texture) {
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
	glm::vec4* uniforms = materialUniforms.getValues();
	uniforms[0] = material->albedo;

	uniforms[1] = glm::vec4(
		material->metallic,
		material->roughness,
		material->ao,
		0.0f);

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
	materialUniforms.submit();

	// submit all material textures (or dummies if they are not set)
	submitSampler(geometryAlbedoSampler, 0, material->albedoTexture);
	submitSampler(geometryMetallicRoughnessSampler, 1, material->metallicRoughnessTexture);
	submitSampler(geometryEmissiveSampler, 2, material->emissiveTexture);
	submitSampler(geometryAoSampler, 3, material->aoTexture);
	submitSampler(geometryNormalSampler, 4, material->normalTexture);
}


//
//	OtSceneRenderer::submitBlendMapUniforms
//

void OtSceneRenderer::submitBlendMapUniforms(OtBlendMapMaterial material) {
	// set the uniform values
	glm::vec4* uniforms = blendmapUniforms.getValues();

	uniforms[0] = glm::vec4(
		material->metallic,
		material->roughness,
		material->scale,
		0.0f);

	uniforms[1] = glm::vec4(
		material->noneNormalsTexture.isReady(),
		material->redNormalsTexture.isReady(),
		material->greenNormalsTexture.isReady(),
		material->blueNormalsTexture.isReady());

	// submit the uniforms
	blendmapUniforms.submit();

	// submit all material textures (or dummies if they are not set/ready)
	submitSampler(blendMapSampler, 0, material->blendMapTexture);

	submitSampler(albedoNoneSampler, 1, material->noneTexture);
	submitSampler(albedoRedSampler, 2, material->redTexture);
	submitSampler(albedoGreenSampler, 3, material->greenTexture);
	submitSampler(albedoBlueSampler, 4, material->blueTexture);

	submitSampler(normalsNoneSampler, 5, material->noneNormalsTexture);
	submitSampler(normalsRedSampler, 6, material->redNormalsTexture);
	submitSampler(normalsGreenSampler, 7, material->greenNormalsTexture);
	submitSampler(normalsBlueSampler, 8, material->blueNormalsTexture);
}


//
//	OtSceneRenderer::submitLightUniforms
//

void OtSceneRenderer::submitLightUniforms(OtScene* scene) {
	// get the directional light information
	glm::vec3 direction = glm::vec3(0.0);
	glm::vec3 color = glm::vec3(0.0);

	for (auto&& [entity, light, transform] : scene->view<OtDirectionalLightComponent, OtTransformComponent>().each()) {
		direction = transform.getTransform()[3];
		color = light.color;
	}

	// build and submit the light uniforms
	glm::vec4* uniforms = lightingUniforms.getValues();
	uniforms[0] = glm::vec4(cameraPosition, 0.0f);
	uniforms[1] = glm::vec4(direction, 0.0f);
	uniforms[2] = glm::vec4(color, 0.0f);
	lightingUniforms.submit();
}
