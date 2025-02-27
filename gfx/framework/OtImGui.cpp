//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
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

#include "OtInteger.h"

#include "OtDejaVu.h"
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
		case GLFW_KEY_F13: return ImGuiKey_F13;
		case GLFW_KEY_F14: return ImGuiKey_F14;
		case GLFW_KEY_F15: return ImGuiKey_F15;
		case GLFW_KEY_F16: return ImGuiKey_F16;
		case GLFW_KEY_F17: return ImGuiKey_F17;
		case GLFW_KEY_F18: return ImGuiKey_F18;
		case GLFW_KEY_F19: return ImGuiKey_F19;
		case GLFW_KEY_F20: return ImGuiKey_F20;
		case GLFW_KEY_F21: return ImGuiKey_F21;
		case GLFW_KEY_F22: return ImGuiKey_F22;
		case GLFW_KEY_F23: return ImGuiKey_F23;
		case GLFW_KEY_F24: return ImGuiKey_F24;
		default: return ImGuiKey_None;
	}
}


//
//	OtFramework::initIMGUI
//

void OtFramework::initIMGUI() {
	// initialize Dear ImGui library
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// deactiviate Dear ImGui automatic .ini file handling
	ImGuiIO& io = ImGui::GetIO();
	io.IniFilename = nullptr;

	// setup backend metadata
 	io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
 	io.BackendPlatformName = "ObjectTalk";
	io.BackendRendererName = "BGFX/GLFW";

	// connect Dear ImGui to GLFW window
	io.DisplaySize = ImVec2(static_cast<float>(width), static_cast<float>(height));
	io.DeltaTime = 1.0f / 60.0f;

	// windows can only be dragged using the title bar
	io.ConfigWindowsMoveFromTitleBarOnly = true;

	// setup main viewport
	ImGui::GetMainViewport()->PlatformHandleRaw = nativeDisplayHandle;

	// setup clipboard functions
	ImGuiPlatformIO& pio = ImGui::GetPlatformIO();
	pio.Platform_ClipboardUserData = this;

	pio.Platform_SetClipboardTextFn = [](ImGuiContext* /* ctx */, const char* text) {
		OtFramework* framework = (OtFramework*) ImGui::GetPlatformIO().Platform_ClipboardUserData;
		framework->clipboard.set(text);
	};

	pio.Platform_GetClipboardTextFn = [](ImGuiContext* /* ctx */) {
		OtFramework* framework = (OtFramework*) ImGui::GetPlatformIO().Platform_ClipboardUserData;
		auto callback = glfwSetErrorCallback(nullptr);
		auto clipboard =framework->clipboard.get();
		framework->clipboardText = clipboard ? clipboard : "";
		glfwSetErrorCallback(callback);
		return framework->clipboardText.c_str();
	};

	// add custom fonts
	ImFontConfig config;
	config.FontDataOwnedByAtlas = false;
	config.OversampleH = 1;
	config.OversampleV = 1;

	static const ImWchar unicodeRanges[] = {
		0x0020, 0x007f, // basic latin
		0x0080, 0x00ff, // latin supplement
		0x0100, 0x017f, // latin extended A
		0x0180, 0x024f, // latin extended B
		0x1e00, 0x1eff, // latin extended additional
		0x20a0, 0x21cf, // currency symbols
		0x2190, 0x21ff, // arrows
		0x2300, 0x23fa, // miscellanious technical
		0x25a0, 0x25ff, // geometric shapes
		0x2700, 0x27ef,	// digbats
		0
	};

	io.Fonts->AddFontFromMemoryCompressedTTF(
		(void*) &OtDejaVu,
		OtDejaVuSize,
		15.0,
		&config,
		unicodeRanges);

	io.Fonts->AddFontFromMemoryCompressedTTF(
		(void*) &OtDejaVu,
		OtDejaVuSize,
		17.0,
		&config,
		unicodeRanges);

	// setup font atlas in texture
	unsigned char* pixels;
	int fw, fh;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &fw, &fh);
	imguiFontTexture.loadFromMemory(fw, fh, OtTexture::rgba8Texture, pixels);
	io.Fonts->TexID = (ImTextureID)(intptr_t) imguiFontTexture.getIndex();

	// setup vertex declaration
	imguiVertexLayout
		.begin()
		.add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
		.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
		.add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
		.end();

	// tweak default style
	auto& style = ImGui::GetStyle();
	style.HoverDelayShort = 0.5f;
	style.HoverDelayNormal = 1.0f;

	style.HoverFlagsForTooltipMouse =
		ImGuiHoveredFlags_Stationary |
		ImGuiHoveredFlags_DelayNormal |
		ImGuiHoveredFlags_AllowWhenDisabled;
}


//
//	OtFramework::frameIMGUI
//

void OtFramework::frameIMGUI() {
	// update Dear ImGui state
	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2(static_cast<float>(width), static_cast<float>(height));
	io.DeltaTime = loopDuration / 1000.0f;

	while (!eventQueue.empty()) {
		auto event = eventQueue.pop();

		switch (event.type) {
			case OtFwEvent::Type::mouseButton:
				io.AddKeyEvent(ImGuiMod_Ctrl, (event.mouseButton.mods & GLFW_MOD_CONTROL) != 0);
				io.AddKeyEvent(ImGuiMod_Shift, (event.mouseButton.mods & GLFW_MOD_SHIFT) != 0);
				io.AddKeyEvent(ImGuiMod_Alt, (event.mouseButton.mods & GLFW_MOD_ALT) != 0);
				io.AddKeyEvent(ImGuiMod_Super, (event.mouseButton.mods & GLFW_MOD_SUPER) != 0);
				io.AddMouseButtonEvent(event.mouseButton.button, event.mouseButton.action == GLFW_PRESS);
				break;

			case OtFwEvent::Type::mouseMove:
				io.AddMousePosEvent(event.mouseMove.x, event.mouseMove.y);
				break;

			case OtFwEvent::Type::mouseWheel:
				io.AddMouseWheelEvent(event.mouseWheel.xOffset, event.mouseWheel.yOffset);
				break;

			case OtFwEvent::Type::keyboard:
				io.AddKeyEvent(ImGuiMod_Ctrl, (event.keyboard.mods & GLFW_MOD_CONTROL) != 0);
				io.AddKeyEvent(ImGuiMod_Shift, (event.keyboard.mods & GLFW_MOD_SHIFT) != 0);
				io.AddKeyEvent(ImGuiMod_Alt, (event.keyboard.mods & GLFW_MOD_ALT) != 0);
				io.AddKeyEvent(ImGuiMod_Super, (event.keyboard.mods & GLFW_MOD_SUPER) != 0);
				io.AddKeyEvent(toImGuiKey(event.keyboard.key), (event.keyboard.action != GLFW_RELEASE));
				break;

			case OtFwEvent::Type::character:
				io.AddInputCharacter(event.character.codepoint);
				break;

			default:
				break;
		}
	}

	// handle cursor which is actually set in main thread because that's where the window manager runs
	setCursor = !(io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange) && glfwGetInputMode(window, GLFW_CURSOR) != GLFW_CURSOR_DISABLED;

	if (setCursor) {
		if (io.MouseDrawCursor) {
			cursor = ImGuiMouseCursor_None;

		} else {
			cursor = ImGui::GetMouseCursor();
		}
	}

	// start a new frame
	ImGui::NewFrame();

	if (metrics) {
		ImGui::ShowMetricsWindow();
	}

	if (demo) {
		ImGui::ShowDemoWindow();
	}
}


//
//	OtFramework::renderIMGUI
//

void OtFramework::renderIMGUI() {
	// render UI to command lists
	ImGui::Render();
	ImDrawData* drawData = ImGui::GetDrawData();

	// Avoid rendering when minimized, scale coordinates for retina displays
	// (screen coordinates != framebuffer coordinates)
	ImGuiIO& io = ImGui::GetIO();
	int fb_width = static_cast<int>(io.DisplaySize.x * io.DisplayFramebufferScale.x);
	int fb_height = static_cast<int>(io.DisplaySize.y * io.DisplayFramebufferScale.y);

	if (fb_width == 0 || fb_height == 0) {
		return;
	}

	drawData->ScaleClipRects(io.DisplayFramebufferScale);

	// setup orthographic projection matrix
	glm::mat4 matrix = glm::ortho(0.0f, io.DisplaySize.x, io.DisplaySize.y, 0.0f);
	bgfx::setViewTransform(255, nullptr, glm::value_ptr(matrix));
	bgfx::setViewRect(255, 0, 0, static_cast<uint16_t>(fb_width), static_cast<uint16_t>(fb_height));

	// Render command lists
	for (int n = 0; n < drawData->CmdListsCount; n++) {
		const ImDrawList* cmd_list = drawData->CmdLists[n];

		bgfx::TransientVertexBuffer tvb;
		bgfx::TransientIndexBuffer tib;

		uint32_t numVertices = static_cast<uint32_t>(cmd_list->VtxBuffer.size());
		uint32_t numIndices = static_cast<uint32_t>(cmd_list->IdxBuffer.size());

		if ((bgfx::getAvailTransientVertexBuffer(numVertices, imguiVertexLayout) != numVertices) ||
			(bgfx::getAvailTransientIndexBuffer(numIndices) != numIndices)) {
			// not enough space in transient buffer, quit drawing the rest...
			break;
		}

		bgfx::allocTransientVertexBuffer(&tvb, numVertices, imguiVertexLayout);
		bgfx::allocTransientIndexBuffer(&tib, numIndices);

		ImDrawVert* verts = (ImDrawVert*) tvb.data;
		memcpy(verts, cmd_list->VtxBuffer.begin(), numVertices * sizeof(ImDrawVert));

		ImDrawIdx* indices = (ImDrawIdx*) tib.data;
		memcpy(indices, cmd_list->IdxBuffer.begin(), numIndices * sizeof(ImDrawIdx));

		for (auto i = 0; i < cmd_list->CmdBuffer.Size; i++) {
			const ImDrawCmd* cmd = &cmd_list->CmdBuffer[i];

			if (cmd->UserCallback) {
				cmd->UserCallback(cmd_list, cmd);

			} else {
				const uint16_t xx = static_cast<uint16_t>(std::max(cmd->ClipRect.x, 0.0f));
				const uint16_t yy = static_cast<uint16_t>(std::max(cmd->ClipRect.y, 0.0f));

				bgfx::setState(
					BGFX_STATE_WRITE_RGB |
					BGFX_STATE_WRITE_A |
					BGFX_STATE_MSAA |
					BGFX_STATE_BLEND_ALPHA);

				bgfx::setScissor(
					xx, yy,
					static_cast<uint16_t>(std::min(cmd->ClipRect.z, 65535.0f) - xx),
					static_cast<uint16_t>(std::min(cmd->ClipRect.w, 65535.0f) - yy));

				bgfx::TextureHandle texture = { (uint16_t)((intptr_t) cmd->TextureId & 0xffff) };
				imguiFontSampler.submit(0, texture);
				bgfx::setVertexBuffer(0, &tvb);
				bgfx::setIndexBuffer(&tib, cmd->IdxOffset, cmd->ElemCount);
				imguiShaderProgram.submit(255);
			}
		}
	}
}


//
//	OtFramework::endIMGUI
//

void OtFramework::endIMGUI() {
	// we have to manually clear our resources since it's too late to let the destructors
	// do it since they run after we shutdown the libraries (causing 'memory leaks')
	imguiFontTexture.clear();
	imguiFontSampler.clear();
	imguiShaderProgram.clear();
	ImGui::DestroyContext();
}
