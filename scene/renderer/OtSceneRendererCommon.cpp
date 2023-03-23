
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
	material->update();

	// submit based on type
	if (material.isKindOf<OtPbrMaterialClass>()) {
		submitPbrUniforms(OtPbrMaterial(material));

	} else if (material.isKindOf<OtBlendMapMaterialClass>()) {
		submitBlendMapUniforms(OtBlendMapMaterial(material));

	} else {
		OtExcept("Internal error: unknown material type [%s]", material->getTypeName());
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
		material->albedoTexture.isValid(),
		material->metallicRoughnessTexture.isValid(),
		0.0f,
		0.0f);

	uniforms[4] = glm::vec4(
		material->emissiveTexture.isValid(),
		material->aoTexture.isValid(),
		material->normalTexture.isValid(),
		0.0);

	// submit the uniforms
	materialUniforms.submit();

	// submit all material textures (or dummies if they are not set)
	geometryAlbedoSampler.submit(0, material->albedoTexture);
	geometryMetallicRoughnessSampler.submit(1, material->metallicRoughnessTexture);
	geometryEmissiveSampler.submit(2, material->emissiveTexture);
	geometryAoSampler.submit(3, material->aoTexture);
	geometryNormalSampler.submit(4, material->normalTexture);
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
		material->normalsNoneTexture.isValid(),
		material->normalsRedTexture.isValid(),
		material->normalsGreenTexture.isValid(),
		material->normalsBlueTexture.isValid());

	// submit the uniforms
	blendmapUniforms.submit();

	// submit all material textures (or dummies if they are not set)
	blendMapSampler.submit(0, material->blendMapTexture);

	albedoNoneSampler.submit(1, material->noneTexture);
	albedoRedSampler.submit(2, material->redTexture);
	albedoGreenSampler.submit(3, material->greenTexture);
	albedoBlueSampler.submit(4, material->blueTexture);

	normalsNoneSampler.submit(5, material->normalsNoneTexture);
	normalsRedSampler.submit(6, material->normalsRedTexture);
	normalsGreenSampler.submit(7, material->normalsGreenTexture);
	normalsBlueSampler.submit(8, material->normalsBlueTexture);
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
