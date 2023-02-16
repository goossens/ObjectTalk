//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"

#include "OtSceneRenderer.h"


//
//	OtSceneRenderer::renderGeometryPass
//

void OtSceneRenderer::renderGeometryPass(OtScene2 scene) {
	// update gbuffer
	gbuffer.update(width, height);

	// setup pass
	OtPass pass;
	pass.reserveRenderingSlot();
	pass.setClear(true, true, 0);
	pass.setRectangle(0, 0, width, height);
	pass.setFrameBuffer(gbuffer);
	pass.setTransform(viewMatrix, projectionMatrix);

	// render all geometries (that have transform and material components)
	for (auto entity : scene->view<OtGeometryComponent, OtTransformComponent, OtMaterialComponent>()) {
		renderGeometry(pass, scene, entity);
	}
}


//
//	OtSceneRenderer::renderGeometry
//

void OtSceneRenderer::renderGeometry(OtPass& pass, OtScene2 scene, OtEntity entity) {
	// get all relevant components
	auto& geometry = scene->getComponent<OtGeometryComponent>(entity);
	auto& material = scene->getComponent<OtMaterialComponent>(entity);
	material.update();

	// submit the geometry
	if (geometry.wireframe) {
		geometry.geometry->submitLines();

	} else {
		geometry.geometry->submitTriangles();
	}

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

	// submit all textures (or dummies if they are not set)
	geometryAlbedoSampler.submit(0, material.albedoTexture);
	geometryMetallicSampler.submit(1, material.metallicTexture);
	geometryRoughnessSampler.submit(2, material.roughnessTexture);
	geometryEmissiveSampler.submit(3, material.emissiveTexture);
	geometryAoSampler.submit(4, material.aoTexture);
	geometryNormalSampler.submit(5, material.normalTexture);

	// set the shader state
	if (geometry.wireframe) {
		geometryShader.setState(
			OtStateWriteRgb |
			OtStateWriteA |
			OtStateWriteZ |
			OtStateDepthTestLess |
			OtStateLines);

	} else if (geometry.cullback) {
		geometryShader.setState(
			OtStateWriteRgb |
			OtStateWriteA |
			OtStateWriteZ |
			OtStateDepthTestLess |
			OtStateCullCw);

	} else {
		geometryShader.setState(
			OtStateWriteRgb |
			OtStateWriteA |
			OtStateWriteZ |
			OtStateDepthTestLess);
	}

	// set the transform
	geometryShader.setTransform(scene->getGlobalTransform(entity));

	// run the shader
	pass.runShader(geometryShader);
}


//
//	OtSceneRenderer::visualizeGbuffer
//

void OtSceneRenderer::visualizeGbuffer() {
	ImVec2 size{gbuffer.getWidth() / 2.0f, gbuffer.getHeight() / 2.0f};
	ImGui::Begin("G-Buffer");

	if (ImGui::BeginTabBar("Textures")) {
		if (ImGui::BeginTabItem("Albado")) {
			ImGui::Image((void*)(intptr_t) gbuffer.getAlbedoTextureIndex(), size);
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Normal")) {
			ImGui::Image((void*)(intptr_t) gbuffer.getNormalTextureIndex(), size);
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("PBR")) {
			ImGui::Image((void*)(intptr_t) gbuffer.getPbrTextureIndex(), size);
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();
}
