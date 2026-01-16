//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>
#include <cstdint>
#include <cstring>

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlgpu3.h"
#include "implot.h"

#include "OtDejaVu.h"
#include "OtFontAudio.h"
#include "OtFramework.h"
#include "OtGpu.h"


//
//	OtFramework::initIMGUI
//

void OtFramework::initIMGUI() {
	// setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImPlot::CreateContext();

	// deactivate Dear ImGui automatic .ini file handling
	auto& io = ImGui::GetIO();
	io.IniFilename = nullptr;

	// windows can only be dragged using the title bar
	io.ConfigWindowsMoveFromTitleBarOnly = true;

	// tweak default style
	ImGui::StyleColorsDark();
	auto& style = ImGui::GetStyle();
	style.HoverDelayShort = 0.5f;
	style.HoverDelayNormal = 1.0f;

	style.HoverFlagsForTooltipMouse =
		ImGuiHoveredFlags_Stationary |
		ImGuiHoveredFlags_DelayNormal |
		ImGuiHoveredFlags_AllowWhenDisabled;

	// setup platform/renderer backends
	auto& gpu = OtGpu::instance();
	ImGui_ImplSDL3_InitForSDLGPU(gpu.window);

	ImGui_ImplSDLGPU3_InitInfo initInfo{};
	initInfo.Device = gpu.device;
	initInfo.ColorTargetFormat = SDL_GetGPUSwapchainTextureFormat(gpu.device, gpu.window);
	initInfo.MSAASamples = SDL_GPU_SAMPLECOUNT_1;
	ImGui_ImplSDLGPU3_Init(&initInfo);

	// setup our fonts
	io.Fonts->Clear();
	ImFontConfig config;

	std::memcpy(config.Name, "DejaVu", 7);
	config.FontDataOwnedByAtlas = false;
	config.OversampleH = 1;
	config.OversampleV = 1;
	io.Fonts->AddFontFromMemoryCompressedTTF(reinterpret_cast<const void*>(OtDejaVu::data()), static_cast<int>(OtDejaVu::size()), 15.0f, &config);

	std::memcpy(config.Name, "FontAudio", 14);
	config.FontDataOwnedByAtlas = false;
	config.OversampleH = 1;
	config.OversampleV = 1;
	io.Fonts->AddFontFromMemoryCompressedTTF(reinterpret_cast<const void*>(OtFontAudio::data()), static_cast<int>(OtFontAudio::size()), 15.0f, &config);
}


//
//	OtFramework::eventIMGUI
//

void OtFramework::eventIMGUI(SDL_Event& event) {
	ImGui_ImplSDL3_ProcessEvent(&event);
}


//
//	OtFramework::startFrameIMGUI
//

void OtFramework::startFrameIMGUI() {
	// start the Dear ImGui frame
	ImGui_ImplSDLGPU3_NewFrame();
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();
}


//
//	OtFramework::endFrameIMGUI
//

void OtFramework::endFrameIMGUI() {
	// render debug windows (if required)
	if (ImGui::IsKeyPressed(ImGuiKey_F16)) {
		profiler = !profiler;

	} else if (ImGui::IsKeyPressed(ImGuiKey_F17)) {
		metrics = !metrics;

	} else if (ImGui::IsKeyPressed(ImGuiKey_F18)) {
		demo = !demo;
	}

	if (profiler) {
		renderProfiler();
	}

	if (metrics) {
		ImGui::ShowMetricsWindow();
	}

	if (demo) {
		ImGui::ShowDemoWindow();
	}

	// render to the screen
	auto& gpu = OtGpu::instance();
	ImGui::Render();
	ImDrawData* drawData = ImGui::GetDrawData();
	const bool isMinimized = (drawData->DisplaySize.x <= 0.0f || drawData->DisplaySize.y <= 0.0f);

	if (gpu.swapChainTexture != nullptr && !isMinimized) {
		// run Dear ImGui copy pass
		ImGui_ImplSDLGPU3_PrepareDrawData(drawData, gpu.pipelineCommandBuffer);

		// setup Dear ImGui render target
		SDL_GPUColorTargetInfo targetInfo{};
		targetInfo.texture = gpu.swapChainTexture;
		targetInfo.clear_color = SDL_FColor{0.0f, 0.0f, 0.0f, 1.0f};
		targetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
		targetInfo.store_op = SDL_GPU_STOREOP_STORE;

		// run Dear ImGui render pass
		SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(gpu.pipelineCommandBuffer, &targetInfo, 1, nullptr);
		ImGui_ImplSDLGPU3_RenderDrawData(drawData, gpu.pipelineCommandBuffer, renderPass);
		SDL_EndGPURenderPass(renderPass);
	}
}


//
//	OtFramework::endIMGUI
//

void OtFramework::endIMGUI() {
	ImGui_ImplSDLGPU3_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImPlot::DestroyContext();
	ImGui::DestroyContext();
}


//
//	OtFramework::renderProfiler
//

void OtFramework::renderProfiler() {
	auto& gpu = OtGpu::instance();
	auto labelWith = ImGui::CalcTextSize("                      ").x;

	ImGui::SetNextWindowPos(ImVec2(10.0f, 10.0f), ImGuiCond_Once);
	ImGui::Begin("Profiler", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Text("Framerate:"); ImGui::SameLine(labelWith); ImGui::Text("%5.1f", 1000.0f / loopDuration);
	ImGui::Text("CPU [ms per frame]:"); ImGui::SameLine(labelWith); ImGui::Text("%5.2f", cpuTime);
	ImGui::Text("GPU [ms per frame]:"); ImGui::SameLine(labelWith); ImGui::Text("%5.2f", gpuTime);
	ImGui::Text("GPU wait [ms]:"); ImGui::SameLine(labelWith); ImGui::Text("%5.2f", gpuWaitTime);
	ImGui::Text("Back buffer width:"); ImGui::SameLine(labelWith); ImGui::Text("%5d", gpu.width);
	ImGui::Text("Back buffer height:"); ImGui::SameLine(labelWith); ImGui::Text("%5d", gpu.height);
	ImGui::Text("Anti-aliasing:"); ImGui::SameLine(labelWith); ImGui::Text("%5d", antiAliasing);

	ImGui::Separator();
	ImGui::Text("Compute passes:"); ImGui::SameLine(labelWith); ImGui::Text("%5d", gpu.computePasses.load());
	ImGui::Text("Compute pipelines:"); ImGui::SameLine(labelWith); ImGui::Text("%5d", gpu.computePipelines.load());
	ImGui::Text("Render passes:"); ImGui::SameLine(labelWith); ImGui::Text("%5d", gpu.renderPasses.load());
	ImGui::Text("Render pipelines:"); ImGui::SameLine(labelWith); ImGui::Text("%5d", gpu.renderPipelines.load());
	ImGui::Text("Render shaders:"); ImGui::SameLine(labelWith); ImGui::Text("%5d", gpu.renderShaders.load());
	ImGui::Text("Vertex buffers:"); ImGui::SameLine(labelWith); ImGui::Text("%5d", gpu.vertexBuffers.load());
	ImGui::Text("Index buffers:"); ImGui::SameLine(labelWith); ImGui::Text("%5d", gpu.indexBuffers.load());
	ImGui::Text("Frame buffers:"); ImGui::SameLine(labelWith); ImGui::Text("%5d", gpu.frameBuffers.load());
	ImGui::Text("G-buffers:"); ImGui::SameLine(labelWith); ImGui::Text("%5d", gpu.gBuffers.load());
	ImGui::Text("Textures:"); ImGui::SameLine(labelWith); ImGui::Text("%5d", gpu.textures.load());
	ImGui::Text("Cubemaps:"); ImGui::SameLine(labelWith); ImGui::Text("%5d", gpu.cubemaps.load());
	ImGui::Text("Samplers:"); ImGui::SameLine(labelWith); ImGui::Text("%5d", gpu.samplers.load());
	ImGui::End();
}
