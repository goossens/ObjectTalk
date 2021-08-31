//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cstring>

#include "bgfx/bgfx.h"
#include "bgfx/embedded_shader.h"
#include "bx/math.h"
#include "bx/timer.h"

#include "imgui_impl_glfw.h"

#include "application.h"
#include "imguishader.h"


//
//	Globals
//

static const bgfx::EmbeddedShader embeddedShaders[] = {
	BGFX_EMBEDDED_SHADER(vs_ocornut_imgui),
	BGFX_EMBEDDED_SHADER(fs_ocornut_imgui),
	BGFX_EMBEDDED_SHADER_END()
};


//
//	OtApplicationClass::initIMGUI
//

void OtApplicationClass::initIMGUI() {
	// initialize ImGui library
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	// deactiviate ImGui automatic .ini file handling
	ImGuiIO& io = ImGui::GetIO();
	io.IniFilename = nullptr;

	// connect IMGUI to GLFW window
	#if __APPLE__
		ImGui_ImplGlfw_InitForOther(window, true);

	#elif define(_WIND32)
		ImGui_ImplGlfw_InitForVulkan(window, true);

	#else
		ImGui_ImplGlfw_InitForOpenGL(window, true);
	#endif

	// Setup vertex declaration
	imguiVertexLayout
		.begin()
		.add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
		.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
		.add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
		.end();

	imguiFontUniform = bgfx::createUniform("g_AttribLocationTex", bgfx::UniformType::Sampler);

	// Create shader program
	bgfx::RendererType::Enum type = bgfx::getRendererType();

	imguiProgram = bgfx::createProgram(
		bgfx::createEmbeddedShader(embeddedShaders, type, "vs_ocornut_imgui"),
		bgfx::createEmbeddedShader(embeddedShaders, type, "fs_ocornut_imgui"),
		true);

	// setup default font
	unsigned char* pixels;
	int fw, fh;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &fw, &fh);

	imguiFontTexture = bgfx::createTexture2D(
		fw, fh, false, 1, bgfx::TextureFormat::BGRA8,
		0, bgfx::copy(pixels, fw * fh * 4));

	io.Fonts->TexID = (void*)(intptr_t) imguiFontTexture.idx;
}


//
//	OtApplicationClass::frameIMGUI
//

void OtApplicationClass::frameIMGUI() {
	// start a new frame
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	// ImGui::ShowDemoWindow();
}


//
//	OtApplicationClass::mouseIMGUI
//

bool OtApplicationClass::mouseIMGUI() {
	return ImGui::GetIO().WantCaptureMouse;
}


//
//	OtApplicationClass::keyboardIMGUI
//

bool OtApplicationClass::keyboardIMGUI() {
	return ImGui::GetIO().WantCaptureKeyboard;
}


//
//	OtApplicationClass::renderIMGUI
//

void OtApplicationClass::renderIMGUI() {
	// render GUI into command
	ImGui::Render();
	ImDrawData* drawData = ImGui::GetDrawData();

	// Avoid rendering when minimized, scale coordinates for retina displays
	// (screen coordinates != framebuffer coordinates)
	ImGuiIO& io = ImGui::GetIO();
	int fb_width = io.DisplaySize.x * io.DisplayFramebufferScale.x;
	int fb_height = io.DisplaySize.y * io.DisplayFramebufferScale.y;

	if (fb_width == 0 || fb_height == 0) {
		return;
	}

	drawData->ScaleClipRects(io.DisplayFramebufferScale);

	// Setup render state: alpha-blending enabled, no face culling,
	 // no depth testing, scissor enabled
	 uint64_t state =
		 BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_MSAA |
		 BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA);

	 const bgfx::Caps* caps = bgfx::getCaps();

	 // Setup viewport, orthographic projection matrix
	 float ortho[16];

	 bx::mtxOrtho(
		 ortho, 0.0f, io.DisplaySize.x, io.DisplaySize.y, 0.0f, 0.0f, 1000.0f,
		 0.0f, caps->homogeneousDepth);

	 bgfx::setViewTransform(255, nullptr, ortho);
	 bgfx::setViewRect(255, 0, 0, fb_width, fb_height);

	 // Render command lists
	 for (int n = 0; n < drawData->CmdListsCount; n++) {
		 const ImDrawList* cmd_list = drawData->CmdLists[n];
		 uint32_t idx_buffer_offset = 0;

		 bgfx::TransientVertexBuffer tvb;
		 bgfx::TransientIndexBuffer tib;

		 uint32_t numVertices = (uint32_t)cmd_list->VtxBuffer.size();
		 uint32_t numIndices = (uint32_t)cmd_list->IdxBuffer.size();

		 if ((numVertices != bgfx::getAvailTransientVertexBuffer(numVertices, imguiVertexLayout)) ||
			 (numIndices != bgfx::getAvailTransientIndexBuffer(numIndices))) {
			 // not enough space in transient buffer, quit drawing the rest...
			 break;
		 }

		 bgfx::allocTransientVertexBuffer(&tvb, numVertices, imguiVertexLayout);
		 bgfx::allocTransientIndexBuffer(&tib, numIndices);

		 ImDrawVert* verts = (ImDrawVert*) tvb.data;
		 memcpy(verts, cmd_list->VtxBuffer.begin(), numVertices * sizeof(ImDrawVert));

		 ImDrawIdx* indices = (ImDrawIdx*) tib.data;
		 memcpy(indices, cmd_list->IdxBuffer.begin(), numIndices * sizeof(ImDrawIdx));

		 for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++) {
			 const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];

			 if (pcmd->UserCallback) {
				 pcmd->UserCallback(cmd_list, pcmd);

			 } else {
				 const uint16_t xx = (uint16_t)bx::max(pcmd->ClipRect.x, 0.0f);
				 const uint16_t yy = (uint16_t)bx::max(pcmd->ClipRect.y, 0.0f);

				 bgfx::setScissor(xx, yy, bx::min(pcmd->ClipRect.z, 65535.0f) - xx, bx::min(pcmd->ClipRect.w, 65535.0f) - yy);
				 bgfx::setState(state);
				 bgfx::TextureHandle texture = { (uint16_t)((intptr_t) pcmd->TextureId & 0xffff) };
				 bgfx::setTexture(0, imguiFontUniform, texture);
				 bgfx::setVertexBuffer(0, &tvb);
				 bgfx::setIndexBuffer(&tib, idx_buffer_offset, pcmd->ElemCount);
				 bgfx::submit(255, imguiProgram);
			 }

			 idx_buffer_offset += pcmd->ElemCount;
		 }
	 }
}


//
//	OtApplicationClass::endIMGUI
//

void OtApplicationClass::endIMGUI() {
	bgfx::destroy(imguiFontUniform);
	bgfx::destroy(imguiFontTexture);
	bgfx::destroy(imguiProgram);
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}
