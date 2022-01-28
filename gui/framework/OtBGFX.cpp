//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cstring>

#include "bx/timer.h"
#include "imgui.h"

#include "OtException.h"

#include "OtFramework.h"


//
//	OtFrameworkClass::initBGFX
//

void OtFrameworkClass::initBGFX() {
	// initialize bgfx
	bgfx::Init init;

#if __APPLE__
	init.type = bgfx::RendererType::Metal;

#elif defined(_WIN32)
	init.type = bgfx::RendererType::Direct3D12;

#else
	init.type = bgfx::RendererType::OpenGLES;
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

	// initialize time management
	startTime = lastTime = bx::getHPCounter();
}


//
//	OtFrameworkClass::frameBGFX
//

void OtFrameworkClass::frameBGFX() {
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

	// start BGFX frame by clearing the screen
	bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x000000ff, 1.0f, 0);
	bgfx::setViewRect(0, 0, 0, width, height);
	bgfx::touch(0);
}


//
//	OtFrameworkClass::renderProfiler
//

void OtFrameworkClass::renderProfiler() {
	const bgfx::Stats* stats = bgfx::getStats();
	const double toMs = 1000.0 / (double) bx::getHPFrequency();

	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	ImGui::Begin("Profiler", nullptr, 0);
	ImGui::Text("Framerate:"); ImGui::SameLine(150); ImGui::Text("%.1f", getFrameRate());
	ImGui::Text("CPU [ms per frame]:"); ImGui::SameLine(150); ImGui::Text("%0.1f", (double) cpuDuration * toMs);
	ImGui::Text("GPU [ms per frame]:"); ImGui::SameLine(150); ImGui::Text("%0.1f", (double) gpuDuration * toMs);
	ImGui::Text("Wait render [ms]:"); ImGui::SameLine(150); ImGui::Text("%0.1f", (double) stats->waitRender * toMs);
	ImGui::Text("Wait submit [ms]:"); ImGui::SameLine(150); ImGui::Text("%0.1f", (double) stats->waitSubmit * toMs);
	ImGui::Text("Backbuffer width:"); ImGui::SameLine(150); ImGui::Text("%d", stats->width);
	ImGui::Text("Backbuffer height:"); ImGui::SameLine(150); ImGui::Text("%d", stats->height);
	ImGui::Text("Anti-aliasing:"); ImGui::SameLine(150); ImGui::Text("%d", antiAliasing);
	ImGui::Text("Views:"); ImGui::SameLine(150); ImGui::Text("%d", nextViewID + 2);
	ImGui::Text("Draw calls:"); ImGui::SameLine(150); ImGui::Text("%d", stats->numDraw);
	ImGui::Text("Programs:"); ImGui::SameLine(150); ImGui::Text("%d", stats->numPrograms);
	ImGui::Text("Shaders:"); ImGui::SameLine(150); ImGui::Text("%d", stats->numShaders);
	ImGui::Text("Textures:"); ImGui::SameLine(150); ImGui::Text("%d", stats->numTextures);
	ImGui::Text("Uniforms:"); ImGui::SameLine(150); ImGui::Text("%d", stats->numUniforms);
	ImGui::Text("Vertex buffers:"); ImGui::SameLine(150); ImGui::Text("%d", stats->numVertexBuffers);
	ImGui::End();
}


//
//	OtFrameworkClass::getTime
//

float OtFrameworkClass::getTime() {
	return (double) (loopTime - startTime) / (double) bx::getHPFrequency();
}


//
//	OtFrameworkClass::renderBGFX
//

void OtFrameworkClass::renderBGFX() {
	// get CPU and GPU statistics
	cpuDuration = bx::getHPCounter() - loopTime;

	const bgfx::Stats* stats = bgfx::getStats();
	gpuDuration = stats->gpuTimeEnd - stats->gpuTimeBegin;

	// render BGFX frame
	bgfx::frame();
}


//
//	OtFrameworkClass::endBGFX
//

void OtFrameworkClass::endBGFX() {
	bgfx::shutdown();
}
