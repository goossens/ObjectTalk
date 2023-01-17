//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>
#include <cstring>

#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "imgui.h"

#include "OtInteger.h"

#include "OtBitstreamVeraMono.h"
#include "OtFramework.h"


//
//	Keyboard decoder ring
//

static ImGuiKey toImGuiKey(int key) {
	switch (key) {
		case GLFW_KEY_TAB: return ImGuiKey_Tab;
		case GLFW_KEY_LEFT: return ImGuiKey_LeftArrow;
		case GLFW_KEY_RIGHT: return ImGuiKey_RightArrow;
		case GLFW_KEY_UP: return ImGuiKey_UpArrow;
		case GLFW_KEY_DOWN: return ImGuiKey_DownArrow;
		case GLFW_KEY_PAGE_UP: return ImGuiKey_PageUp;
		case GLFW_KEY_PAGE_DOWN: return ImGuiKey_PageDown;
		case GLFW_KEY_HOME: return ImGuiKey_Home;
		case GLFW_KEY_END: return ImGuiKey_End;
		case GLFW_KEY_INSERT: return ImGuiKey_Insert;
		case GLFW_KEY_DELETE: return ImGuiKey_Delete;
		case GLFW_KEY_BACKSPACE: return ImGuiKey_Backspace;
		case GLFW_KEY_SPACE: return ImGuiKey_Space;
		case GLFW_KEY_ENTER: return ImGuiKey_Enter;
		case GLFW_KEY_ESCAPE: return ImGuiKey_Escape;
		case GLFW_KEY_APOSTROPHE: return ImGuiKey_Apostrophe;
		case GLFW_KEY_COMMA: return ImGuiKey_Comma;
		case GLFW_KEY_MINUS: return ImGuiKey_Minus;
		case GLFW_KEY_PERIOD: return ImGuiKey_Period;
		case GLFW_KEY_SLASH: return ImGuiKey_Slash;
		case GLFW_KEY_SEMICOLON: return ImGuiKey_Semicolon;
		case GLFW_KEY_EQUAL: return ImGuiKey_Equal;
		case GLFW_KEY_LEFT_BRACKET: return ImGuiKey_LeftBracket;
		case GLFW_KEY_BACKSLASH: return ImGuiKey_Backslash;
		case GLFW_KEY_RIGHT_BRACKET: return ImGuiKey_RightBracket;
		case GLFW_KEY_GRAVE_ACCENT: return ImGuiKey_GraveAccent;
		case GLFW_KEY_CAPS_LOCK: return ImGuiKey_CapsLock;
		case GLFW_KEY_SCROLL_LOCK: return ImGuiKey_ScrollLock;
		case GLFW_KEY_NUM_LOCK: return ImGuiKey_NumLock;
		case GLFW_KEY_PRINT_SCREEN: return ImGuiKey_PrintScreen;
		case GLFW_KEY_PAUSE: return ImGuiKey_Pause;
		case GLFW_KEY_KP_0: return ImGuiKey_Keypad0;
		case GLFW_KEY_KP_1: return ImGuiKey_Keypad1;
		case GLFW_KEY_KP_2: return ImGuiKey_Keypad2;
		case GLFW_KEY_KP_3: return ImGuiKey_Keypad3;
		case GLFW_KEY_KP_4: return ImGuiKey_Keypad4;
		case GLFW_KEY_KP_5: return ImGuiKey_Keypad5;
		case GLFW_KEY_KP_6: return ImGuiKey_Keypad6;
		case GLFW_KEY_KP_7: return ImGuiKey_Keypad7;
		case GLFW_KEY_KP_8: return ImGuiKey_Keypad8;
		case GLFW_KEY_KP_9: return ImGuiKey_Keypad9;
		case GLFW_KEY_KP_DECIMAL: return ImGuiKey_KeypadDecimal;
		case GLFW_KEY_KP_DIVIDE: return ImGuiKey_KeypadDivide;
		case GLFW_KEY_KP_MULTIPLY: return ImGuiKey_KeypadMultiply;
		case GLFW_KEY_KP_SUBTRACT: return ImGuiKey_KeypadSubtract;
		case GLFW_KEY_KP_ADD: return ImGuiKey_KeypadAdd;
		case GLFW_KEY_KP_ENTER: return ImGuiKey_KeypadEnter;
		case GLFW_KEY_KP_EQUAL: return ImGuiKey_KeypadEqual;
		case GLFW_KEY_LEFT_SHIFT: return ImGuiKey_LeftShift;
		case GLFW_KEY_LEFT_CONTROL: return ImGuiKey_LeftCtrl;
		case GLFW_KEY_LEFT_ALT: return ImGuiKey_LeftAlt;
		case GLFW_KEY_LEFT_SUPER: return ImGuiKey_LeftSuper;
		case GLFW_KEY_RIGHT_SHIFT: return ImGuiKey_RightShift;
		case GLFW_KEY_RIGHT_CONTROL: return ImGuiKey_RightCtrl;
		case GLFW_KEY_RIGHT_ALT: return ImGuiKey_RightAlt;
		case GLFW_KEY_RIGHT_SUPER: return ImGuiKey_RightSuper;
		case GLFW_KEY_MENU: return ImGuiKey_Menu;
		case GLFW_KEY_0: return ImGuiKey_0;
		case GLFW_KEY_1: return ImGuiKey_1;
		case GLFW_KEY_2: return ImGuiKey_2;
		case GLFW_KEY_3: return ImGuiKey_3;
		case GLFW_KEY_4: return ImGuiKey_4;
		case GLFW_KEY_5: return ImGuiKey_5;
		case GLFW_KEY_6: return ImGuiKey_6;
		case GLFW_KEY_7: return ImGuiKey_7;
		case GLFW_KEY_8: return ImGuiKey_8;
		case GLFW_KEY_9: return ImGuiKey_9;
		case GLFW_KEY_A: return ImGuiKey_A;
		case GLFW_KEY_B: return ImGuiKey_B;
		case GLFW_KEY_C: return ImGuiKey_C;
		case GLFW_KEY_D: return ImGuiKey_D;
		case GLFW_KEY_E: return ImGuiKey_E;
		case GLFW_KEY_F: return ImGuiKey_F;
		case GLFW_KEY_G: return ImGuiKey_G;
		case GLFW_KEY_H: return ImGuiKey_H;
		case GLFW_KEY_I: return ImGuiKey_I;
		case GLFW_KEY_J: return ImGuiKey_J;
		case GLFW_KEY_K: return ImGuiKey_K;
		case GLFW_KEY_L: return ImGuiKey_L;
		case GLFW_KEY_M: return ImGuiKey_M;
		case GLFW_KEY_N: return ImGuiKey_N;
		case GLFW_KEY_O: return ImGuiKey_O;
		case GLFW_KEY_P: return ImGuiKey_P;
		case GLFW_KEY_Q: return ImGuiKey_Q;
		case GLFW_KEY_R: return ImGuiKey_R;
		case GLFW_KEY_S: return ImGuiKey_S;
		case GLFW_KEY_T: return ImGuiKey_T;
		case GLFW_KEY_U: return ImGuiKey_U;
		case GLFW_KEY_V: return ImGuiKey_V;
		case GLFW_KEY_W: return ImGuiKey_W;
		case GLFW_KEY_X: return ImGuiKey_X;
		case GLFW_KEY_Y: return ImGuiKey_Y;
		case GLFW_KEY_Z: return ImGuiKey_Z;
		case GLFW_KEY_F1: return ImGuiKey_F1;
		case GLFW_KEY_F2: return ImGuiKey_F2;
		case GLFW_KEY_F3: return ImGuiKey_F3;
		case GLFW_KEY_F4: return ImGuiKey_F4;
		case GLFW_KEY_F5: return ImGuiKey_F5;
		case GLFW_KEY_F6: return ImGuiKey_F6;
		case GLFW_KEY_F7: return ImGuiKey_F7;
		case GLFW_KEY_F8: return ImGuiKey_F8;
		case GLFW_KEY_F9: return ImGuiKey_F9;
		case GLFW_KEY_F10: return ImGuiKey_F10;
		case GLFW_KEY_F11: return ImGuiKey_F11;
		case GLFW_KEY_F12: return ImGuiKey_F12;
		default: return ImGuiKey_None;
	}
}


//
//	OtFrameworkClass::initIMGUI
//

void OtFrameworkClass::initIMGUI() {
	// initialize ImGui library
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsClassic();
//	ImGui::StyleColorsDark();

	// deactiviate ImGui automatic .ini file handling
	ImGuiIO& io = ImGui::GetIO();
	io.IniFilename = nullptr;

	// connect IMGUI to GLFW window
	io.DisplaySize = ImVec2(width, height);
	io.DeltaTime = 1.0f / 60.0f;

	// setup main viewport
	ImGui::GetMainViewport()->PlatformHandleRaw = nativeDisplayHandle;

	// setup clipboard functions
	io.ClipboardUserData = this;

	io.SetClipboardTextFn = [](void* data, const char* text) {
		OtFrameworkClass* framework = (OtFrameworkClass*) data;
		glfwSetClipboardString(framework->window, text);
	};

	io.GetClipboardTextFn = [](void* data) {
		OtFrameworkClass* framework = (OtFrameworkClass*) data;
		return glfwGetClipboardString(framework->window);
	};

	// add custom font
	ImFontConfig config;
	config.FontDataOwnedByAtlas = false;

	io.Fonts->AddFontFromMemoryTTF(
		(void*) &OtBitstreamVeraMono,
		sizeof(OtBitstreamVeraMono),
		16.0,
		&config);

	// setup font atlas in texture
	unsigned char* pixels;
	int fw, fh;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &fw, &fh);
	imguiFontTexture.loadFromMemory(fw, fh, pixels);
	io.Fonts->TexID = (void*)(intptr_t) imguiFontTexture.getTextureIndex();

	// setup vertex declaration
	imguiVertexLayout
		.begin()
		.add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
		.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
		.add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
		.end();

	// initialize sampler and shader
	imguiFontSampler.initialize("g_AttribLocationTex");
	imguiShader.initialize("OtImGuiVS", "OtImGuiFS");
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
				io.AddKeyEvent(ImGuiMod_Ctrl, (event.mouseButton.mods & GLFW_MOD_CONTROL) != 0);
				io.AddKeyEvent(ImGuiMod_Shift, (event.mouseButton.mods & GLFW_MOD_SHIFT) != 0);
				io.AddKeyEvent(ImGuiMod_Alt, (event.mouseButton.mods & GLFW_MOD_ALT) != 0);
				io.AddKeyEvent(ImGuiMod_Super, (event.mouseButton.mods & GLFW_MOD_SUPER) != 0);
				io.AddMouseButtonEvent(event.mouseButton.button, event.mouseButton.action == GLFW_PRESS);
				break;

			case OtFwEvent::mouseMoveEvent:
				io.AddMousePosEvent(event.mouseMove.x, event.mouseMove.y);
				break;

			case OtFwEvent::mouseDragEvent:
				io.AddMousePosEvent(event.mouseDrag.x, event.mouseDrag.y);
				break;

			case OtFwEvent::mouseWheelEvent:
				io.AddMouseWheelEvent(event.mouseWheel.xOffset, event.mouseWheel.yOffset);
				break;

			case OtFwEvent::keyboardEvent:
				io.AddKeyEvent(ImGuiMod_Ctrl, (event.keyboard.mods & GLFW_MOD_CONTROL) != 0);
				io.AddKeyEvent(ImGuiMod_Shift, (event.keyboard.mods & GLFW_MOD_SHIFT) != 0);
				io.AddKeyEvent(ImGuiMod_Alt, (event.keyboard.mods & GLFW_MOD_ALT) != 0);
				io.AddKeyEvent(ImGuiMod_Super, (event.keyboard.mods & GLFW_MOD_SUPER) != 0);
				io.AddKeyEvent(toImGuiKey(event.keyboard.key), (event.keyboard.action == GLFW_PRESS));
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

	// filter out events if ImGui wants them
	if (io.WantCaptureMouse) {
		events.erase(std::remove_if(events.begin(), events.end(), [] (OtFwEvent& event) {
			return event.isMouseEvent();
		}), events.end());
	}

	if (io.WantCaptureKeyboard) {
		events.erase(std::remove_if(events.begin(), events.end(), [] (OtFwEvent& event) {
			return event.isKeyboardEvent();
		}), events.end());
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

		uint32_t numVertices = (uint32_t) cmd_list->VtxBuffer.size();
		uint32_t numIndices = (uint32_t) cmd_list->IdxBuffer.size();

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
				imguiFontSampler.submit(0, texture);
				bgfx::setVertexBuffer(0, &tvb);
				bgfx::setIndexBuffer(&tib, pcmd->IdxOffset, pcmd->ElemCount);
				imguiShader.submit(255);
			}
		}
	}
}


//
//	OtFrameworkClass::endIMGUI
//

void OtFrameworkClass::endIMGUI() {
	// we have to manually clear our resources since it's too late to let the destructors
	// do it since they run after we shutdown the libraries (causing 'memory leaks')
	imguiFontTexture.clear();
	imguiFontSampler.clear();
	imguiShader.clear();

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
