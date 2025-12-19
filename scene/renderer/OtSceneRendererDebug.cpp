//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cstdint>
#include <string>

#include "fmt/format.h"
#include "imgui.h"

#include "OtAssetManager.h"
#include "OtPath.h"
#include "OtRenderPass.h"
#include "OtSampler.h"
#include "OtVertex.h"
#include "OtUi.h"

#include "OtSceneRendererDebug.h"
#include "OtShaders.h"


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

		if (ibl.iblBrdfLut.isValid()) {
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
		renderer.deferredRenderingBuffer.get ## part ## TextureID(),		\
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
				renderTexture(title.c_str(), renderer.ctx.csm.getDepthTexture(i).getTextureID(), size, size);
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
			renderTexture("Occlusion Buffer", postProcessor.occlusionBuffer);

		} else {
			ImGui::SeparatorText("No Data");
		}
	}
}


//
//	OtSceneRendererDebug::renderTimings
//

void OtSceneRendererDebug::renderTimings(OtSceneRenderer& renderer) {
	if (ImGui::CollapsingHeader("CPU Timings (ms)")) {
		OtUi::readonlyFloat("Context setup", renderer.ctxTime);
		OtUi::readonlyFloat("Shadow pass", renderer.shadowPassTime);
		OtUi::readonlyFloat("Background pass", renderer.backgroundPassTime);
		OtUi::readonlyFloat("Opaque pass", renderer.opaquePassTime);
		OtUi::readonlyFloat("Water pass", renderer.waterPassTime);
		OtUi::readonlyFloat("Sky pass", renderer.skyPassTime);
		OtUi::readonlyFloat("Particle pass", renderer.particlePassTime);
		OtUi::readonlyFloat("Transparent pass", renderer.transparentPassTime);
		OtUi::readonlyFloat("Grid pass", renderer.gridPassTime);
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

void OtSceneRendererDebug::renderTexture(const char* title, ImTextureID index, int width, int height) {
	if (ImGui::TreeNode(title)) {
		auto size = ImGui::GetContentRegionAvail().x;
		ImGui::Image(index, ImVec2(size, size * height / width));
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

		if (cubemap.hasMip()) {
			OtUi::dragInt("Mip Level", &debug.requestedMip, 0, cubemap.getMipLevels() - 1);
		}

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
	// initialize resources (if required)
	if (!resourcesInitialized) {
		initializeResources();
		resourcesInitialized = true;
	}

	// set framebuffer size
	static constexpr int width = 600;
	static constexpr int height = width * 4 / 6;
	debug.framebuffer.update(width, height);

	// create mipmap sampler
	OtSampler sampler;
	sampler.setFilter(OtSampler::Filter::linear);
	sampler.setAddressing(OtSampler::Addressing::clamp);
	auto mipmapLevels = cubemap.getMipLevels();
	sampler.setMinMaxLod(0.0f, static_cast<float>(mipmapLevels ? mipmapLevels - 1 : 0));

	// start a rendering pass
	OtRenderPass pass;
	pass.setClearColor(true);
	pass.start(debug.framebuffer);
	pass.bindPipeline(pipeline);
	pass.bindFragmentSampler(0, sampler, cubemap);

	// set vertext uniforms
	glm::mat4 view = glm::scale(glm::mat4(1.0f), glm::vec3(width / 2.0f, height / 1.5f, 1.0f));
	glm::mat4 projection = glm::ortho(0.0f, float(width), float(height), 0.0f);

	struct VertexUniforms {
		glm::mat4 viewProjectionMatrix;
	} vertexUniforms {
		projection * view
	};

	pass.bindVertexUniforms(0, &vertexUniforms, sizeof(vertexUniforms));

	// set fragment uniforms
	struct FragmentUniforms {
		float mipLevel;
	} fragmentUniforms {
		static_cast<float>(debug.requestedMip)
	};

	pass.bindFragmentUniforms(0, &fragmentUniforms, sizeof(fragmentUniforms));

	// render cubemap as cross
	pass.render(vertexBuffer, indexBuffer);
	pass.end();

	// update metadata
	debug.renderedVersion = cubemap.getVersion();
	debug.renderedMip = debug.requestedMip;
}


//
//	OtSceneRendererDebug::initializeResources
//

void OtSceneRendererDebug::initializeResources() {
	pipeline.setShaders(OtCubeMapCrossVert, OtCubeMapCrossVertSize, OtCubeMapCrossFrag, OtCubeMapCrossFragSize);
	pipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::rgba8);
	pipeline.setVertexDescription(OtVertexPosUvw::getDescription());

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

	vertexBuffer.set(crossVertices, sizeof(crossVertices) / sizeof(*crossVertices) / 6, OtVertexPosUvw::getDescription());

	static uint32_t crossIndices[] = {
		0, 1, 3, 3, 1, 4,
		2, 3, 6, 6, 3, 7,
		3, 4, 7, 7, 4, 8,
		4, 5, 8, 8, 5, 9,
		7, 8, 10, 10, 8, 11,
		10, 11, 12, 12, 11, 13
	};

	indexBuffer.set(crossIndices, sizeof(crossIndices) / sizeof(*crossIndices));
	resourcesInitialized = true;
}
