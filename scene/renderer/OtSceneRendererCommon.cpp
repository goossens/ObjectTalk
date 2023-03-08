
//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtSceneRenderer.h"


//
//	OtSceneRenderer::submitMaterialUniforms
//

void OtSceneRenderer::submitMaterialUniforms(std::shared_ptr<OtScene> scene, OtEntity entity) {
	// get the material component
	auto& material = scene->getComponent<OtMaterialComponent>(entity);
	material.update();

	// set the uniform values
	glm::vec4* uniforms = materialUniforms.getValues();
	uniforms[0] = glm::vec4(material.albedo, 0.0f);

	uniforms[1] = glm::vec4(
		material.metallic,
		material.roughness,
		material.emissive,
		material.ao);

	uniforms[2] = glm::vec4(
		material.albedoTexture.isValid(),
		material.metallicTexture.isValid(),
		material.roughnessTexture.isValid(),
		0.0f);

	uniforms[3] = glm::vec4(
		material.emissiveTexture.isValid(),
		material.aoTexture.isValid(),
		material.normalTexture.isValid(),
		0.0);

	materialUniforms.submit();

	// submit all material textures (or dummies if they are not set)
	geometryAlbedoSampler.submit(0, material.albedoTexture);
	geometryMetallicSampler.submit(1, material.metallicTexture);
	geometryRoughnessSampler.submit(2, material.roughnessTexture);
	geometryEmissiveSampler.submit(3, material.emissiveTexture);
	geometryAoSampler.submit(4, material.aoTexture);
	geometryNormalSampler.submit(5, material.normalTexture);
}


//
//	OtSceneRenderer::submitLightUniforms
//

void OtSceneRenderer::submitLightUniforms(std::shared_ptr<OtScene> scene) {
	// get the directional light information
	glm::vec3 direction = glm::vec3(0.0);
	glm::vec3 color = glm::vec3(0.0);

	for (auto&& [entity, light, transform] : scene->view<OtDirectionalLightComponent, OtTransformComponent>().each()) {
		direction = transform.getTransform()[3];
		color = light.color;
	}

	// build and submit the uniforms
	glm::vec4* uniforms = lightingUniforms.getValues();
	uniforms[0] = glm::vec4(cameraPosition, 0.0f);
	uniforms[1] = glm::vec4(direction, 0.0f);
	uniforms[2] = glm::vec4(color, 0.0f);
	lightingUniforms.submit();
}
