//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"

#include "OtPass.h"
#include "OtTransientIndexBuffer.h"
#include "OtTransientVertexBuffer.h"
#include "OtVertex.h"

#include "OtSceneRendererDebug.h"


//
//	OtSceneRendererDebug::render
//

void OtSceneRendererDebug::render(OtSceneRenderer& renderer) {
	auto size = ImGui::GetIO().DisplaySize;
	ImGui::SetNextWindowPos(ImVec2(size.x / 10.0f, size.y / 10.0f), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(size.x * 0.3f, size.y * 0.7f), ImGuiCond_Once);

	ImGui::Begin("Scene Renderer Debug", nullptr, ImGuiWindowFlags_NoCollapse);
	renderIbl(renderer);
	renderReflection(renderer);
	ImGui::End();
}


//
//	OtSceneRendererDebug::renderIbl
//

void OtSceneRendererDebug::renderIbl(OtSceneRenderer& renderer) {
	if (ImGui::CollapsingHeader("Image Based Lighting (IBL)")) {
		if (renderer.iblEnvironmentMap.isValid()) {
			if (renderer.iblSkyMap->isValid()) { renderCubeMap("Sky Map", *renderer.iblSkyMap, iblSkyMapDebug); }
			if (renderer.iblIrradianceMap.isValid()) { renderCubeMap("Irrandiance Map", renderer.iblIrradianceMap, iblIrradianceDebug); }
			if (renderer.iblEnvironmentMap.isValid()) { renderCubeMap("Environment Map", renderer.iblEnvironmentMap, iblEnvironmentDebug); }
			if (renderer.iblBrdfLut.isValid()) { renderTexture("BRDF LUT", renderer.iblBrdfLut); }

		} else {
			ImGui::SeparatorText("No Data");
		}
	}
}


//
//	OtSceneRendererDebug::renderReflection
//

void OtSceneRendererDebug::renderReflection(OtSceneRenderer& renderer) {
	if (ImGui::CollapsingHeader("Reflection/Refraction")) {
		if (renderer.reflectionBuffer.isValid()) {
			if (renderer.reflectionBuffer.isValid()) {
				auto texture = renderer.reflectionBuffer.getColorTexture();
				renderTexture("Reflection Buffer", texture);
			}

			if (renderer.refractionBuffer.isValid()) {
				auto texture = renderer.refractionBuffer.getColorTexture();
				renderTexture("Refraction Buffer", texture);
			}

		} else {
			ImGui::SeparatorText("No Data");
		}
	}
}


//
//	OtSceneRendererDebug::renderTexture
//

void OtSceneRendererDebug::renderTexture(const char* title, OtTexture& texture) {
	if (ImGui::TreeNode(title)) {
		int size = ImGui::GetContentRegionAvail().x;
		int width = texture.getWidth();
		int height = texture.getHeight();
		ImGui::Image((void*)(intptr_t) texture.getIndex(), ImVec2(size, size * height / width));
		ImGui::TreePop();
	}
}


//
//	OtSceneRendererDebug::renderCubeMap
//

void OtSceneRendererDebug::renderCubeMap(const char* title, OtCubeMap& cubemap, CubeMapDebug& debug) {
	if (ImGui::TreeNode(title)) {
		ImGui::SliderInt("Mip Level", &debug.requestedMip, 0, cubemap.getMipLevels());

		if (cubemap.getVersion() != debug.renderedVersion || debug.requestedMip != debug.renderedMip) {
			renderCubeMapAsCross(cubemap, debug);
		}

		int size = ImGui::GetContentRegionAvail().x;
		uint16_t index = debug.framebuffer.getColorTexture().getIndex();
		ImGui::Image((void*)(intptr_t) index, ImVec2(size, size * 3 / 4));
		ImGui::TreePop();
	}
}


//
//	OtSceneRendererDebug::renderCubeMapAsCross
//

void OtSceneRendererDebug::renderCubeMapAsCross(OtCubeMap& cubemap, CubeMapDebug& debug) {
	static float crossVertices[] = {
		0.0f, 0.5f, 0.0f, -1.0f,  1.0f, -1.0f,
		0.0f, 1.0f, 0.0f, -1.0f, -1.0f, -1.0f,

		0.5f, 0.0f, 0.0f, -1.0f,  1.0f, -1.0f,
		0.5f, 0.5f, 0.0f, -1.0f,  1.0f,  1.0f,
		0.5f, 1.0f, 0.0f, -1.0f, -1.0f,  1.0f,
		0.5f, 1.5f, 0.0f, -1.0f, -1.0f, -1.0f,

		1.0f, 0.0f, 0.0f,  1.0f,  1.0f, -1.0f,
		1.0f, 0.5f, 0.0f,  1.0f,  1.0f,  1.0f,
		1.0f, 1.0f, 0.0f,  1.0f, -1.0f,  1.0f,
		1.0f, 1.5f, 0.0f,  1.0f, -1.0f, -1.0f,

		1.5f, 0.5f, 0.0f,  1.0f,  1.0f, -1.0f,
		1.5f, 1.0f, 0.0f,  1.0f, -1.0f, -1.0f,

		2.0f, 0.5f, 0.0f, -1.0f,  1.0f, -1.0f,
		2.0f, 1.0f, 0.0f, -1.0f, -1.0f, -1.0f,
	};

	static constexpr size_t crossVertexCount = sizeof(crossVertices) / sizeof(*crossVertices) / 6;

	static uint32_t crossIndices[] = {
		0, 1, 3, 3, 1, 4,
		2, 3, 6, 6, 3, 7,
		3, 4, 7, 7, 4, 8,
		4, 5, 8, 8, 5, 9,
		7, 8, 10, 10, 8, 11,
		10, 11, 12, 12, 11, 13
	};

	static constexpr size_t crossIndexCount = sizeof(crossIndices) / sizeof(*crossIndices);

	// set framebufer size
	static constexpr int width = 600;
	static constexpr int height = width * 4 / 6;
	debug.framebuffer.update(width, height);

	// start a rendering pass
	OtPass pass;
	pass.setClear(true, false);
	pass.setRectangle(0, 0, width, height);
	pass.setFrameBuffer(debug.framebuffer);

	// setup projection
	glm::mat4 view = glm::scale(glm::mat4(1.0f), glm::vec3(width / 2.0f, height / 1.5f, 1.0f));
	glm::mat4 projection = glm::ortho(0.0f, float(width), float(height), 0.0f, -1.0f, 1.0f);
	pass.setTransform(view, projection);

	// submit geometry for cross
	OtTransientVertexBuffer tvb;
	OtTransientIndexBuffer tib;
	tvb.submit(crossVertices, crossVertexCount, OtVertexPosUvw::getLayout());
	tib.submit(crossIndices, crossIndexCount);

	// setup sampler and uniform
	crossSampler.submit(0, cubemap.getHandle());
	crossUniform.setValue(0, float(debug.requestedMip), 0.0f, 0.0f, 0.0f);
	crossUniform.submit();

	// run program
	pass.runShaderProgram(crossShader);

	// update metadata
	debug.renderedVersion = cubemap.getVersion();
	debug.renderedMip = debug.requestedMip;
}