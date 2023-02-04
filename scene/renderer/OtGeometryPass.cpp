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
	// setup pass
	geometryPass.reserveRenderingSlot();
	geometryPass.setClear(true, true, 0);
	geometryPass.setRectangle(0, 0, width, height);
	geometryPass.setFrameBuffer(gbuffer);
	geometryPass.setTransform(camera->getViewMatrix(), camera->getProjectionMatrix());

	// render all geometries
	for (auto entity : scene->view<OtGeometryComponent>()) {
		if (scene->hasComponent<OtTransformComponent>(entity) && scene->hasComponent<OtMaterialComponent>(entity)) {
			renderGeometry(scene, entity);
		}
	}
}




//
//	OtSceneRenderer::renderGeometry
//

void OtSceneRenderer::renderGeometry(OtScene2 scene, OtEntity entity) {
	// get all relevant components
	auto& geometry = scene->getComponent<OtGeometryComponent>(entity);
	auto& transform = scene->getComponent<OtTransformComponent>(entity);
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
	uniforms[0] = material.albedo;

	uniforms[1] = glm::vec4(
		material.metallic,
		material.roughness,
		material.ao,
		material.albedoTexture.isValid());

	uniforms[2] = glm::vec4(
		material.metallicTexture.isValid(),
		material.roughnessTexture.isValid(),
		material.aoTexture.isValid(),
		material.normalTexture.isValid());

	materialUniforms.submit();

	// submit all textures (or dummies if they are not set)
	geometryAlbedoSampler.submit(0, material.albedoTexture);
	geometryMetallicSampler.submit(1, material.metallicTexture);
	geometryRoughnessSampler.submit(2, material.roughnessTexture);
	geometryAoSampler.submit(3, material.aoTexture);
	geometryNormalSampler.submit(4, material.normalTexture);

	// load the shader (if required)
	if (!geometryShader.isValid()) {
		geometryShader.initialize("OtGeometryVS", "OtGeometryFS");
	}

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
			OtStateDepthTestLess |
			OtStateMsaa);
	}

	// set the model transform
	auto matrix = transform.getTransform();
	geometryShader.setTransform(matrix);

	// run the shader
	geometryPass.runShader(geometryShader);
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

		if (ImGui::BeginTabItem("Position")) {
			ImGui::Image((void*)(intptr_t) gbuffer.getPositionTextureIndex(), size);
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
