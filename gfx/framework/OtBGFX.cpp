//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cstring>

#include "bx/timer.h"
#include "imgui.h"
#include "mipmap.h"

#include "OtException.h"
#include "OtFormat.h"

#include "OtFramework.h"
#include "OtPass.h"


//
//	OtFramework::initBGFX
//

void OtFramework::initBGFX() {
	// initialize bgfx
	bgfx::Init init;

#if __APPLE__
	init.type = bgfx::RendererType::Metal;

#elif defined(_WIN32)
	init.type = bgfx::RendererType::Direct3D12;

#else
	init.type = bgfx::RendererType::OpenGL;
#endif

	init.platformData.nwh = nativeDisplayHandle;
	init.platformData.ndt = nativeDisplayType;
	init.resolution.width  = width;
	init.resolution.height = height;
	init.resolution.reset  = BGFX_RESET_VSYNC;
	init.debug = false;

	bgfx::init(init);

	// ensure this platform has the right capabilities
	const bgfx::Caps* caps = bgfx::getCaps();

	if (!(caps->supported & BGFX_CAPS_RENDERER_MULTITHREADED)) {
		OtExcept("Your system/graphics card does not support multithreading");
	}

	if (!(caps->supported & BGFX_CAPS_INDEX32)) {
		OtExcept("Your system/graphics card does not support 32 bit vertex indexing");
	}

	if (!(caps->supported & BGFX_CAPS_INSTANCING)) {
		OtExcept("Your system/graphics card does not support instancing");
	}

	if (!(caps->supported & BGFX_CAPS_TEXTURE_COMPARE_LEQUAL)) {
		OtExcept("Your system/graphics card does not support texture '<='");
	}

	if (!(caps->supported & BGFX_CAPS_TEXTURE_BLIT)) {
		OtExcept("Your system/graphics card does not support blitting between textures");
	}

	// initialize time management
	startTime = lastTime = bx::getHPCounter();
}


//
//	OtFramework::frameBGFX
//

void OtFramework::frameBGFX() {
	// get time since epoch
	loopTime = bx::getHPCounter();

	// calculate loop speed
	if (loopTime >= lastTime) {
		loopDuration = (double) (loopTime - lastTime) / (double) bx::getHPFrequency() * 1000.0;

	} else {
		loopDuration = 1.0 / 60.0 * 1000.0;
	}

	lastTime = loopTime;

	// reset graphic settings and back-buffer size (if required)
	static int prevWidth = 0;
	static int prevHeight = 0;
	static int prevAA = 0;

	if (width != prevWidth || height != prevHeight || antiAliasing != prevAA) {
		int AA;

		switch (antiAliasing) {
			case 1: AA = BGFX_RESET_MSAA_X2; break;
			case 2: AA = BGFX_RESET_MSAA_X4; break;
			case 3: AA = BGFX_RESET_MSAA_X8; break;
			case 4: AA = BGFX_RESET_MSAA_X16; break;
			default: AA = 0; break;
		}

		bgfx::reset(width, height, BGFX_RESET_VSYNC | AA);
		prevWidth = width;
		prevHeight = height;
		prevAA = antiAliasing;
	}

	// start BGFX frame by clearing the backbuffer
	bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x000000ff, 1.0f, 0);
	bgfx::setViewRect(0, 0, 0, width, height);
	bgfx::touch(0);
}


//
//	OtFramework::renderProfiler
//

void OtFramework::renderProfiler() {
	const bgfx::Stats* stats = bgfx::getStats();
	const double toMsCpu = 1000.0 / stats->cpuTimerFreq;
	const double toMsGpu = 1000.0 / stats->gpuTimerFreq;
	auto labelWith = ImGui::CalcTextSize("                     ").x;

	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	ImGui::Begin("Profiler", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Text("Framerate:"); ImGui::SameLine(labelWith); ImGui::Text("%.1f", getFrameRate());
	ImGui::Text("CPU [ms per frame]:"); ImGui::SameLine(labelWith); ImGui::Text("%0.1f", (double) (stats->cpuTimeEnd - stats->cpuTimeBegin) * toMsCpu);
	ImGui::Text("GPU [ms per frame]:"); ImGui::SameLine(labelWith); ImGui::Text("%0.1f", (double) (stats->gpuTimeEnd - stats->gpuTimeBegin) * toMsGpu);
	ImGui::Text("Wait render [ms]:"); ImGui::SameLine(labelWith); ImGui::Text("%0.1f", (double) stats->waitRender * toMsCpu);
	ImGui::Text("Wait submit [ms]:"); ImGui::SameLine(labelWith); ImGui::Text("%0.1f", (double) stats->waitSubmit * toMsCpu);
	ImGui::Text("Backbuffer width:"); ImGui::SameLine(labelWith); ImGui::Text("%d", stats->width);
	ImGui::Text("Backbuffer height:"); ImGui::SameLine(labelWith); ImGui::Text("%d", stats->height);
	ImGui::Text("Anti-aliasing:"); ImGui::SameLine(labelWith); ImGui::Text("%d", antiAliasing);
	ImGui::Text("Views:"); ImGui::SameLine(labelWith); ImGui::Text("%d", OtPassCount() + 2);
	ImGui::Text("Draw calls:"); ImGui::SameLine(labelWith); ImGui::Text("%d", stats->numDraw);
	ImGui::Text("Programs:"); ImGui::SameLine(labelWith); ImGui::Text("%d", stats->numPrograms);
	ImGui::Text("Shaders:"); ImGui::SameLine(labelWith); ImGui::Text("%d", stats->numShaders);
	ImGui::Text("Textures:"); ImGui::SameLine(labelWith); ImGui::Text("%d", stats->numTextures);
	ImGui::Text("Uniforms:"); ImGui::SameLine(labelWith); ImGui::Text("%d", stats->numUniforms);
	ImGui::Text("Vertex buffers:"); ImGui::SameLine(labelWith); ImGui::Text("%d", stats->numVertexBuffers);
	ImGui::End();
}


//
//	OtFramework::getTime
//

float OtFramework::getTime() {
	return (double) (loopTime - startTime) / (double) bx::getHPFrequency();
}


//
//	OtFramework::renderBGFX
//

void OtFramework::renderBGFX() {
	// render BGFX frame
	bgfx::frame();
}


//
//	OtFramework::endBGFX
//

void OtFramework::endBGFX() {
	// shutdown BGFX
	bgfx::shutdown();
}
