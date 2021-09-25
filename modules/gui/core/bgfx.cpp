//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cstring>

#include "bx/timer.h"
#include "bgfx/platform.h"

#include "application.h"
#include "theme.h"


//
//	Globals
//

double OtApplicationClass::lastTime;
double OtApplicationClass::loopTime;
double OtApplicationClass::loopDuration;

double OtApplicationClass::cpuDuration;
double OtApplicationClass::gpuDuration;


//
//	OtApplicationClass::initBGFX
//

void OtApplicationClass::initBGFX() {
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

	bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x000000ff, 1.0f, 0);
	bgfx::setViewRect(0, 0, 0, width, height);

	lastTime = double(bx::getHPCounter()) / double(bx::getHPFrequency());
}


//
//	OtApplicationClass::frameBGFX
//

void OtApplicationClass::frameBGFX() {
	// get time since epoch
	loopTime = double(bx::getHPCounter()) / double(bx::getHPFrequency());

	// calculate loop speed
	loopDuration = loopTime - lastTime;
	lastTime = loopTime;

	// start BGFX frame
	bgfx::reset(width, height, BGFX_RESET_VSYNC | antiAliasing);
	bgfx::setViewRect(0, 0, 0, width, height);
	bgfx::touch(0);
}


//
//	OtApplicationClass::renderProfiler
//

void OtApplicationClass::renderProfiler() {
	const bgfx::Stats* stats = bgfx::getStats();
	const double toMs = 1000.0 / double(bx::getHPFrequency());

	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	ImGui::Begin("Profiler", nullptr, 0);
	ImGui::Text("Framerate:"); ImGui::SameLine(150); ImGui::Text("%.1f", getFrameRate());
	ImGui::Text("CPU [ms per frame]:"); ImGui::SameLine(150); ImGui::Text("%0.1f", cpuDuration * 1000.0);
	ImGui::Text("GPU [ms per frame]:"); ImGui::SameLine(150); ImGui::Text("%0.1f", gpuDuration * 1000.0);
	ImGui::Text("Wait render [ms]:"); ImGui::SameLine(150); ImGui::Text("%0.1f", double(stats->waitRender) * toMs);
	ImGui::Text("Wait submit [ms]:"); ImGui::SameLine(150); ImGui::Text("%0.1f", double(stats->waitSubmit) * toMs);
	ImGui::Text("Backbuffer width:"); ImGui::SameLine(150); ImGui::Text("%d", stats->width);
	ImGui::Text("Backbuffer height:"); ImGui::SameLine(150); ImGui::Text("%d", stats->height);
	ImGui::Text("Anti-aliasing:"); ImGui::SameLine(150); ImGui::Text("%d", antiAliasing);
	ImGui::Text("Draw calls:"); ImGui::SameLine(150); ImGui::Text("%d", stats->numDraw);
	ImGui::Text("Programs:"); ImGui::SameLine(150); ImGui::Text("%d", stats->numPrograms);
	ImGui::Text("Shaders:"); ImGui::SameLine(150); ImGui::Text("%d", stats->numShaders);
	ImGui::Text("Textures:"); ImGui::SameLine(150); ImGui::Text("%d", stats->numTextures);
	ImGui::Text("Uniforms:"); ImGui::SameLine(150); ImGui::Text("%d", stats->numUniforms);
	ImGui::Text("Vertex buffers:"); ImGui::SameLine(150); ImGui::Text("%d", stats->numVertexBuffers);
	ImGui::Text("Index buffers:"); ImGui::SameLine(150); ImGui::Text("%d", stats->numIndexBuffers);
	ImGui::End();
}


//
//	OtApplicationClass::renderBGFX
//

void OtApplicationClass::renderBGFX() {
	// get CPU and GPU statistics
	cpuDuration = double(bx::getHPCounter()) / double(bx::getHPFrequency()) - loopTime;

	const bgfx::Stats* stats = bgfx::getStats();
	gpuDuration = double(stats->gpuTimeEnd - stats->gpuTimeBegin) / double(stats->gpuTimerFreq);

	// render BGFX frame
	bgfx::frame();
}


//
//	OtApplicationClass::endBGFX
//

void OtApplicationClass::endBGFX() {
	bgfx::shutdown();
}
