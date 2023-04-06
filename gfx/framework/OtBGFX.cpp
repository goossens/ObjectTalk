//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cstring>
#include <string>

#include "bx/timer.h"
#include "imgui.h"
#include "mipmap.h"

#include "OtException.h"
#include "OtFormat.h"
#include "OtLog.h"

#include "OtFramework.h"
#include "OtPass.h"


//
//	BGFX Callback Interface
//

struct BgfxCallback : public bgfx::CallbackI {
	virtual ~BgfxCallback() {}

	virtual void fatal(const char* filePath, uint16_t line, bgfx::Fatal::Enum code, const char* str) override {
		OtLogFatal(OtFormat("Fatal error: 0x%08x: %s", code, str));
	}

	virtual void traceVargs(const char* filePath, uint16_t line, const char* format, va_list argList) override {
		char buffer[256];
		auto size = std::vsnprintf(buffer, 256, format, argList);

		if (buffer[size - 1] == '\n') {
			buffer[size - 1] = 0;
		}

		OtLogger::instance()->log(filePath, line, OtLogger::debug, std::string(buffer));
	}

	virtual void profilerBegin(const char* name, uint32_t abgr, const char* filePath, uint16_t line) override {}
	virtual void profilerBeginLiteral(const char* name, uint32_t abgr, const char* filePath, uint16_t line) override {}
	virtual void profilerEnd() override {}
	virtual uint32_t cacheReadSize(uint64_t id) override { return 0; }
	virtual bool cacheRead(uint64_t id, void* data, uint32_t size) override { return false; }
	virtual void cacheWrite(uint64_t id, const void* data, uint32_t size) override {}
	virtual void screenShot(const char* filePath, uint32_t width, uint32_t height, uint32_t pitch, const void* data, uint32_t size, bool yflip) override {}
	virtual void captureBegin(uint32_t width, uint32_t height, uint32_t pitch, bgfx::TextureFormat::Enum format, bool yflip) override {}
	virtual void captureEnd() override {}
	virtual void captureFrame(const void* data, uint32_t size) override {}
};

static BgfxCallback callbacks;


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
	init.resolution.width = width;
	init.resolution.height = height;
	init.resolution.reset = BGFX_RESET_VSYNC;
	init.callback = &callbacks;

	bgfx::init(init);

	// ensure this platform has the right capabilities
	const bgfx::Caps* caps = bgfx::getCaps();

	if (!(caps->supported & BGFX_CAPS_RENDERER_MULTITHREADED)) {
		OtLogFatal("Your system/graphics card does not support multithreading");
	}

	if (!(caps->supported & BGFX_CAPS_INDEX32)) {
		OtLogFatal("Your system/graphics card does not support 32 bit vertex indexing");
	}

	if (!(caps->supported & BGFX_CAPS_INSTANCING)) {
		OtLogFatal("Your system/graphics card does not support instancing");
	}

	if (!(caps->supported & BGFX_CAPS_TEXTURE_COMPARE_LEQUAL)) {
		OtLogFatal("Your system/graphics card does not support texture '<='");
	}

	if (!(caps->supported & BGFX_CAPS_TEXTURE_BLIT)) {
		OtLogFatal("Your system/graphics card does not support blitting between textures");
	}

	// initialize time management
	lastTime = bx::getHPCounter();
}


//
//	OtFramework::frameBGFX
//

void OtFramework::frameBGFX() {
	// get time since epoch
	loopTime = bx::getHPCounter();

	// calculate loop speed
	if (loopTime >= lastTime) {
		loopDuration = (float) ((double) (loopTime - lastTime) / (double) bx::getHPFrequency() * 1000.0);

	} else {
		loopDuration = 1.0f / 60.0f * 1000.0f;
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
	auto caps = bgfx::getCaps();
	auto stats = bgfx::getStats();
	float toMsCpu = 1000.0f / stats->cpuTimerFreq;
	float toMsGpu = 1000.0f / stats->gpuTimerFreq;
	auto labelWith = ImGui::CalcTextSize("                         ").x;

	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	ImGui::Begin("Profiler", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Text("Framerate:"); ImGui::SameLine(labelWith); ImGui::Text("%.1f", 1000.0f / loopDuration);
	ImGui::Text("CPU [ms per frame]:"); ImGui::SameLine(labelWith); ImGui::Text("%0.2f", cpuTime);
	ImGui::Text("GPU [ms per frame]:"); ImGui::SameLine(labelWith); ImGui::Text("%0.2f", (float) (stats->gpuTimeEnd - stats->gpuTimeBegin) * toMsGpu);
	ImGui::Text("Wait render [ms]:"); ImGui::SameLine(labelWith); ImGui::Text("%0.1f", (float) stats->waitRender * toMsCpu);
	ImGui::Text("Wait submit [ms]:"); ImGui::SameLine(labelWith); ImGui::Text("%0.1f", (float) stats->waitSubmit * toMsCpu);
	ImGui::Text("Backbuffer width:"); ImGui::SameLine(labelWith); ImGui::Text("%d", stats->width);
	ImGui::Text("Backbuffer height:"); ImGui::SameLine(labelWith); ImGui::Text("%d", stats->height);
	ImGui::Text("Anti-aliasing:"); ImGui::SameLine(labelWith); ImGui::Text("%d", antiAliasing);
	ImGui::Text("Views:"); ImGui::SameLine(labelWith); ImGui::Text("%d of %d", OtPassCount() + 2, caps->limits.maxViews);
	ImGui::Text("Blits:"); ImGui::SameLine(labelWith); ImGui::Text("%d of %d", stats->numBlit, caps->limits.maxBlits);
	ImGui::Text("Draw calls:"); ImGui::SameLine(labelWith); ImGui::Text("%d of %d", stats->numDraw, caps->limits.maxDrawCalls);
	ImGui::Text("Compute calls:"); ImGui::SameLine(labelWith); ImGui::Text("%d of %d", stats->numCompute, caps->limits.maxComputeBindings);
	ImGui::Text("Programs:"); ImGui::SameLine(labelWith); ImGui::Text("%d of %d", stats->numPrograms, caps->limits.maxPrograms);
	ImGui::Text("Shaders:"); ImGui::SameLine(labelWith); ImGui::Text("%d of %d", stats->numShaders, caps->limits.maxShaders);
	ImGui::Text("Textures:"); ImGui::SameLine(labelWith); ImGui::Text("%d of %d", stats->numTextures, caps->limits.maxTextures);
	ImGui::Text("Uniforms:"); ImGui::SameLine(labelWith); ImGui::Text("%d of %d", stats->numUniforms, caps->limits.maxUniforms);
	ImGui::Text("Vertex buffers:"); ImGui::SameLine(labelWith); ImGui::Text("%d of %d", stats->numVertexBuffers, caps->limits.maxVertexBuffers);
	ImGui::Text("Index buffers:"); ImGui::SameLine(labelWith); ImGui::Text("%d of %d", stats->numIndexBuffers, caps->limits.maxIndexBuffers);
	ImGui::Text("Dynamic Vertex buffers:"); ImGui::SameLine(labelWith); ImGui::Text("%d of %d", stats->numDynamicVertexBuffers, caps->limits.maxDynamicVertexBuffers);
	ImGui::Text("Dynamic Index buffers:"); ImGui::SameLine(labelWith); ImGui::Text("%d of %d", stats->numDynamicIndexBuffers, caps->limits.maxDynamicIndexBuffers);
	ImGui::End();
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
