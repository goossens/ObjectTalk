//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <string>

#include "fmt/format.h"

#include "OtAssetManager.h"
#include "OtPath.h"
#include "OtPass.h"
#include "OtTransientIndexBuffer.h"
#include "OtTransientVertexBuffer.h"
#include "OtVertex.h"
#include "OtUi.h"

#include "OtSceneRendererDebug.h"


//
//	OtSceneRendererDebug::render
//

void OtSceneRendererDebug::render(OtSceneRenderer& renderer) {
	// render the debug window
	auto size = ImGui::GetMainViewport()->Size;
	ImGui::SetNextWindowPos(ImVec2(size.x / 10.0f, size.y / 10.0f), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(size.x * 0.3f, size.y * 0.7f), ImGuiCond_Once);

	ImGui::Begin("Scene Renderer Debug", nullptr, ImGuiWindowFlags_NoCollapse);
	renderIbl(renderer);
	renderGbuffer(renderer);
	renderShadowMaps(renderer);
	renderReflection(renderer);
	renderOcclusion(renderer);
	renderTimings(renderer);
	renderAssets();
	ImGui::End();
}


//
//	OtSceneRendererDebug::renderIbl
//

void OtSceneRendererDebug::renderIbl(OtSceneRenderer& renderer) {
	if (ImGui::CollapsingHeader("Image Based Lighting (IBL)")) {
		auto& ibl = renderer.ctx.ibl;

		if (ibl.iblEnvironmentMap.isValid()) {
			if (ibl.iblSkyMap->isValid()) { renderCubeMap("Sky Map", *ibl.iblSkyMap, iblSkyMapDebug); }
			if (ibl.iblIrradianceMap.isValid()) { renderCubeMap("Irradiance Map", ibl.iblIrradianceMap, iblIrradianceDebug); }
			if (ibl.iblEnvironmentMap.isValid()) { renderCubeMap("Environment Map", ibl.iblEnvironmentMap, iblEnvironmentDebug); }
			if (ibl.iblBrdfLut.isValid()) { renderTexture("BRDF LUT", ibl.iblBrdfLut); }

		} else {
			ImGui::SeparatorText("No Data");
		}
	}
}


//
//	OtSceneRendererDebug::renderGbuffer
//

#define RENDER_GBUFFER(title, part)											\
	renderTexture(															\
		title,																\
		renderer.deferredRenderingBuffer.get ## part ## TextureIndex(),		\
		renderer.deferredRenderingBuffer.getWidth(),						\
		renderer.deferredRenderingBuffer.getHeight())

void OtSceneRendererDebug::renderGbuffer(OtSceneRenderer& renderer) {
	if (ImGui::CollapsingHeader("GBuffer")) {
		if (renderer.deferredRenderingBuffer.isValid()) {
			RENDER_GBUFFER("Albedo", Albedo);
			RENDER_GBUFFER("Normal", Normal);
			RENDER_GBUFFER("PBR", Pbr);
			RENDER_GBUFFER("Emissive", Emissive);

		} else {
			ImGui::SeparatorText("No Data");
		}
	}
}


//
//	OtSceneRendererDebug::renderShadowMaps
//

void OtSceneRendererDebug::renderShadowMaps(OtSceneRenderer& renderer) {
	// shadow maps are created using an orthographic projection
	// this means the depth buffer is linear and doesn't need to be transformed
	if (ImGui::CollapsingHeader("Cascaded Shadow Map")) {
		if (renderer.ctx.csm.isValid()) {
			auto size = renderer.ctx.csm.getSize();

			for (size_t i = 0; i < OtCascadedShadowMap::maxCascades; i++) {
				auto title = fmt::format("Cascade {}", i + 1);
				renderTexture(title.c_str(), renderer.ctx.csm.getDepthTextureIndex(i), size, size);
			}

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
		auto& water = renderer.waterPass;

		if (water.reflectionBuffer.isValid()) {
			if (water.reflectionBuffer.isValid()) {
				auto texture = water.reflectionBuffer.getColorTexture();
				renderTexture("Reflection Buffer", texture);
			}

			if (water.refractionBuffer.isValid()) {
				auto texture = water.refractionBuffer.getColorTexture();
				renderTexture("Refraction Buffer", texture);
			}

		} else {
			ImGui::SeparatorText("No Data");
		}
	}
}


//
//	OtSceneRendererDebug::renderOcclusion
//

void OtSceneRendererDebug::renderOcclusion(OtSceneRenderer& renderer) {
	if (ImGui::CollapsingHeader("Occlusion")) {
		auto& postProcessor = renderer.postProcessingPass;

		if (postProcessor.occlusionBuffer.isValid()) {
			auto texture = postProcessor.occlusionBuffer.getColorTexture();
			renderTexture("Occlusion Buffer", texture);

		} else {
			ImGui::SeparatorText("No Data");
		}
	}
}


//
//	OtSceneRendererDebug::renderTimings
//

void OtSceneRendererDebug::renderTimings(OtSceneRenderer& renderer) {
	if (ImGui::CollapsingHeader("Timings")) {
		OtUi::readonlyFloat("Context setup", renderer.ctxTime);
		OtUi::readonlyFloat("Shadow pass", renderer.shadowPassTime);
		OtUi::readonlyFloat("Background pass", renderer.backgroundPassTime);
		OtUi::readonlyFloat("Opaque pass", renderer.opaquePassTime);
		OtUi::readonlyFloat("Transparent pass", renderer.transparentPassTime);
		OtUi::readonlyFloat("Sky pass", renderer.skyPassTime);
		OtUi::readonlyFloat("Water pass", renderer.waterPassTime);
		OtUi::readonlyFloat("Particle pass", renderer.particlePassTime);
		OtUi::readonlyFloat("Post Processing", renderer.postProcessingTime);
		OtUi::readonlyFloat("Editor pass", renderer.editorPassTime);
		OtUi::readonlyFloat("Total CPU time", renderer.renderTime);
	}
}


//
//	OtSceneRendererDebug::renderAssets
//

void OtSceneRendererDebug::renderAssets() {
	if (ImGui::CollapsingHeader("Asset Manager")) {
		ImGuiTableFlags flags =
			ImGuiTableFlags_Borders |
			ImGuiTableFlags_Sortable |
			ImGuiTableFlags_Resizable |
			ImGuiTableFlags_SizingStretchProp;

		if (ImGui::BeginTable("assets", 3, flags)) {
			ImGui::TableSetupColumn("Path", 0, 6.0f);
			ImGui::TableSetupColumn("State", 0, 1.0f);
			ImGui::TableSetupColumn("Refs", 0, 1.0f);
			ImGui::TableHeadersRow();

			OtAssetManager::each([](OtAssetBase* asset) {
				auto path = asset->getPath();
				auto name = OtPath::getFilename(path);

				ImGui::TableNextRow();
				ImGui::TableNextColumn(); ImGui::TextUnformatted(name.c_str());

				if (ImGui::IsItemHovered(ImGuiHoveredFlags_ForTooltip)) {
					ImGui::BeginTooltip();
					ImGui::TextUnformatted(path.c_str());
					ImGui::EndTooltip();
				}

				ImGui::TableNextColumn(); ImGui::TextUnformatted(asset->getStateName());
				ImGui::TableNextColumn(); ImGui::TextUnformatted(std::to_string(asset->getReferences()).c_str());
			});

			ImGui::EndTable();
		}
	}
}


//
//	OtSceneRendererDebug::renderTexture
//

void OtSceneRendererDebug::renderTexture(const char* title, ImTextureID id, int width, int height) {
	if (ImGui::TreeNode(title)) {
		auto size = ImGui::GetContentRegionAvail().x;
		ImGui::Image(id, ImVec2(size, size * height / width));
		ImGui::TreePop();
	}
}

void OtSceneRendererDebug::renderTexture(const char* title, OtTexture& texture) {
	if (ImGui::TreeNode(title)) {
		auto size = ImGui::GetContentRegionAvail().x;
		int width = texture.getWidth();
		int height = texture.getHeight();
		ImGui::Image(texture.getTextureID(), ImVec2(size, size * height / width));
		ImGui::TreePop();
	}
}


//
//	OtSceneRendererDebug::renderCubeMap
//

void OtSceneRendererDebug::renderCubeMap(const char* title, OtCubeMap& cubemap, CubeMapDebug& debug) {
	if (ImGui::TreeNode(title)) {
		OtUi::dragInt("Mip Level", &debug.requestedMip, 0, cubemap.getMipLevels());

		if (cubemap.getVersion() != debug.renderedVersion || debug.requestedMip != debug.renderedMip) {
			renderCubeMapAsCross(cubemap, debug);
		}

		auto size = ImGui::GetContentRegionAvail().x;
		ImGui::Image(debug.framebuffer.getColorTextureID(), ImVec2(size, size * 3.0f / 4.0f));
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

	// set framebuffer size
	static constexpr int width = 600;
	static constexpr int height = width * 4 / 6;
	debug.framebuffer.update(width, height);

	// start a rendering pass
	OtPass pass;
	pass.setClear(true);
	pass.setRectangle(0, 0, width, height);
	pass.setFrameBuffer(debug.framebuffer);

	// setup projection
	glm::mat4 view = glm::scale(glm::mat4(1.0f), glm::vec3(width / 2.0f, height / 1.5f, 1.0f));
	glm::mat4 projection = glm::ortho(0.0f, float(width), float(height), 0.0f);
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
