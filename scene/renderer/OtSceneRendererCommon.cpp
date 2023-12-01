
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
