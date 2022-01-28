//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cstring>

#include "bgfx/embedded_shader.h"
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "imgui.h"

#include "OtInteger.h"

#include "OtFramework.h"
#include "OtImGuiShader.h"


//
//	Globals
//

static const bgfx::EmbeddedShader embeddedShaders[] = {
	BGFX_EMBEDDED_SHADER(OtImGuiVS),
	BGFX_EMBEDDED_SHADER(OtImGuiFS),
	BGFX_EMBEDDED_SHADER_END()
};


//
//	OtFrameworkClass::initIMGUI
//

void OtFrameworkClass::initIMGUI() {
	// initialize ImGui library
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	// deactiviate ImGui automatic .ini file handling
	ImGuiIO& io = ImGui::GetIO();
	io.IniFilename = nullptr;

	// connect IMGUI to GLFW window
	io.DisplaySize = ImVec2(width, height);
	io.DeltaTime = 1.0f / 60.0f;

	// setup keyboard mapping
	io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
	io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
	io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
	io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
	io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
	io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
	io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
	io.KeyMap[ImGuiKey_Insert] = GLFW_KEY_INSERT;
	io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
	io.KeyMap[ImGuiKey_Space] = GLFW_KEY_SPACE;
	io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
	io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
	io.KeyMap[ImGuiKey_KeyPadEnter] = GLFW_KEY_KP_ENTER;
	io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
	io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
	io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
	io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
	io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
	io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

	// setup clipboard functions
	io.ClipboardUserData = this;

	io.SetClipboardTextFn = [](void* data, const char* text) {
		OtFrameworkClass* app = (OtFrameworkClass*) data;
		glfwSetClipboardString(app->window, text);
	};

	io.GetClipboardTextFn = [](void* data) {
		OtFrameworkClass* app = (OtFrameworkClass*) data;
		return glfwGetClipboardString(app->window);
	};

	// add default font
	io.Fonts->AddFontDefault();

	// setup font atlas in texture
	unsigned char* pixels;
	int fw, fh;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &fw, &fh);

	imguiFontTexture = bgfx::createTexture2D(
		fw, fh, false, 1, bgfx::TextureFormat::BGRA8,
		0, bgfx::copy(pixels, fw * fh * 4));

	io.Fonts->TexID = (void*)(intptr_t) imguiFontTexture.idx;

	// setup vertex declaration
	imguiVertexLayout
		.begin()
		.add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
		.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
		.add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
		.end();

	// setup font texture
	imguiFontUniform = bgfx::createUniform("g_AttribLocationTex", bgfx::UniformType::Sampler);

	// create shader program
	bgfx::RendererType::Enum type = bgfx::getRendererType();

	imguiProgram = bgfx::createProgram(
		bgfx::createEmbeddedShader(embeddedShaders, type, "OtImGuiVS"),
		bgfx::createEmbeddedShader(embeddedShaders, type, "OtImGuiFS"),
		true);
}


//
//	OtFrameworkClass::frameIMGUI
//

void OtFrameworkClass::frameIMGUI(std::vector<OtFwEvent>& events) {
	// update ImGui state
	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2(width, height);
	io.DeltaTime = loopDuration / 1000.0;

	// get information from events
	for (auto& event : events) {
		switch (event.type) {
			case OtFwEvent::mouseButtonEvent:
				io.MouseDown[event.mouseButton.button] = event.mouseButton.action == GLFW_PRESS;
				break;

			case OtFwEvent::mouseMoveEvent:
				io.MousePos = ImVec2(event.mouseMove.x, event.mouseMove.y);
				break;

			case OtFwEvent::mouseDragEvent:
				io.MousePos = ImVec2(event.mouseDrag.x, event.mouseDrag.y);
				break;

			case OtFwEvent::mouseWheelEvent:
				io.MouseWheel = event.mouseWheel.yOffset;
				io.MouseWheelH = event.mouseWheel.xOffset;
				break;

			case OtFwEvent::keyboardEvent:
				io.KeysDown[event.keyboard.key] = event.keyboard.action != GLFW_RELEASE;
				io.KeyShift = event.keyboard.mods & GLFW_MOD_SHIFT;
				io.KeyCtrl = event.keyboard.mods & GLFW_MOD_CONTROL;
				io.KeyAlt = event.keyboard.mods & GLFW_MOD_ALT;
				io.KeySuper = event.keyboard.mods & GLFW_MOD_SUPER;
				break;

			case OtFwEvent::characterEvent:
				io.AddInputCharacter(event.character.codepoint);
				break;

			default:
				break;
		}
	}

	// start a new frame
	ImGui::NewFrame();

	// filter out events if ImGui wants them (use std::remove_if in C++20)
	if (io.WantCaptureMouse) {
		auto event = events.begin();

		while (event != events.end()) {
			if (event->isMouseEvent()) {
				event = events.erase(event);

			} else {
				event++;
			}
		}
	}

	if (io.WantCaptureKeyboard) {
		auto event = events.begin();

		while (event != events.end()) {
			if (event->isKeyboardEvent()) {
				event = events.erase(event);

			} else {
				event++;
			}
		}
	}

	if (demo) {
		ImGui::ShowDemoWindow();
	}
}


//
//	OtFrameworkClass::renderIMGUI
//

void OtFrameworkClass::renderIMGUI() {
	// render GUI to command lists
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

	// setup orthographic projection matrix
	glm::mat4 matrix;
	matrix = glm::ortho(0.0f, io.DisplaySize.x, io.DisplaySize.y, 0.0f, -1.0f, 1.0f);
	bgfx::setViewTransform(255, nullptr, glm::value_ptr(matrix));
	bgfx::setViewRect(255, 0, 0, fb_width, fb_height);

	// Render command lists
	for (int n = 0; n < drawData->CmdListsCount; n++) {
		const ImDrawList* cmd_list = drawData->CmdLists[n];

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
				const uint16_t xx = (uint16_t) bx::max(pcmd->ClipRect.x, 0.0f);
				const uint16_t yy = (uint16_t) bx::max(pcmd->ClipRect.y, 0.0f);

				bgfx::setScissor(xx, yy, bx::min(pcmd->ClipRect.z, 65535.0f) - xx, bx::min(pcmd->ClipRect.w, 65535.0f) - yy);
				bgfx::setState(state);
				bgfx::TextureHandle texture = { (uint16_t)((intptr_t) pcmd->TextureId & 0xffff) };
				bgfx::setTexture(0, imguiFontUniform, texture);
				bgfx::setVertexBuffer(0, &tvb);
				bgfx::setIndexBuffer(&tib, pcmd->IdxOffset, pcmd->ElemCount);
				bgfx::submit(255, imguiProgram);
			}
		}
	}
}


//
//	OtFrameworkClass::endIMGUI
//

void OtFrameworkClass::endIMGUI() {
	bgfx::destroy(imguiFontUniform);
	bgfx::destroy(imguiFontTexture);
	bgfx::destroy(imguiProgram);
	ImGui::DestroyContext();
}


//
//	OtFrameworkClass::addEnumsIMGUI
//

void OtFrameworkClass::addEnumsIMGUI(OtObject module) {
	module->set("windowNoTitleBar", OtIntegerClass::create(ImGuiWindowFlags_NoTitleBar));
	module->set("windowNoResize", OtIntegerClass::create(ImGuiWindowFlags_NoResize));
	module->set("windowNoMove", OtIntegerClass::create(ImGuiWindowFlags_NoMove));
	module->set("windowNoScrollbar", OtIntegerClass::create(ImGuiWindowFlags_NoScrollbar));
	module->set("windowNoScrollWithMouse", OtIntegerClass::create(ImGuiWindowFlags_NoScrollWithMouse));
	module->set("windowNoCollapse", OtIntegerClass::create(ImGuiWindowFlags_NoCollapse));
	module->set("windowAlwaysAutoResize", OtIntegerClass::create(ImGuiWindowFlags_AlwaysAutoResize));
	module->set("windowNoSavedSettings", OtIntegerClass::create(ImGuiWindowFlags_NoSavedSettings));
	module->set("windowNoMouseInputs", OtIntegerClass::create(ImGuiWindowFlags_NoMouseInputs));
	module->set("windowMenuBar", OtIntegerClass::create(ImGuiWindowFlags_MenuBar));
	module->set("windowHorizontalScrollbar", OtIntegerClass::create(ImGuiWindowFlags_HorizontalScrollbar));
	module->set("windowNoFocusOnAppearing", OtIntegerClass::create(ImGuiWindowFlags_NoFocusOnAppearing));
	module->set("windowNoBringToFrontOnFocus", OtIntegerClass::create(ImGuiWindowFlags_NoBringToFrontOnFocus));
	module->set("windowAlwaysVerticalScrollbar", OtIntegerClass::create(ImGuiWindowFlags_AlwaysVerticalScrollbar));
	module->set("windowAlwaysHorizontalScrollbar", OtIntegerClass::create(ImGuiWindowFlags_AlwaysHorizontalScrollbar));
	module->set("windowAlwaysUseWindowPadding", OtIntegerClass::create(ImGuiWindowFlags_AlwaysUseWindowPadding));
	module->set("windowNoNavInputs", OtIntegerClass::create(ImGuiWindowFlags_NoNavInputs));
	module->set("windowNoNavFocus", OtIntegerClass::create(ImGuiWindowFlags_NoNavFocus));
	module->set("windowUnsavedDocument", OtIntegerClass::create(ImGuiWindowFlags_UnsavedDocument));
	module->set("windowNoNav", OtIntegerClass::create(ImGuiWindowFlags_NoNav));
	module->set("windowNoDecoration", OtIntegerClass::create(ImGuiWindowFlags_NoDecoration));
}
