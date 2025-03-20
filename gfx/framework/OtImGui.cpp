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

static ImGuiKey toImGuiKey(SDL_Keycode keycode, SDL_Scancode scancode) {
	// keypad doesn't have individual key values in SDL3
	switch (scancode) {
		case SDL_SCANCODE_KP_0: return ImGuiKey_Keypad0;
		case SDL_SCANCODE_KP_1: return ImGuiKey_Keypad1;
		case SDL_SCANCODE_KP_2: return ImGuiKey_Keypad2;
		case SDL_SCANCODE_KP_3: return ImGuiKey_Keypad3;
		case SDL_SCANCODE_KP_4: return ImGuiKey_Keypad4;
		case SDL_SCANCODE_KP_5: return ImGuiKey_Keypad5;
		case SDL_SCANCODE_KP_6: return ImGuiKey_Keypad6;
		case SDL_SCANCODE_KP_7: return ImGuiKey_Keypad7;
		case SDL_SCANCODE_KP_8: return ImGuiKey_Keypad8;
		case SDL_SCANCODE_KP_9: return ImGuiKey_Keypad9;
		case SDL_SCANCODE_KP_PERIOD: return ImGuiKey_KeypadDecimal;
		case SDL_SCANCODE_KP_DIVIDE: return ImGuiKey_KeypadDivide;
		case SDL_SCANCODE_KP_MULTIPLY: return ImGuiKey_KeypadMultiply;
		case SDL_SCANCODE_KP_MINUS: return ImGuiKey_KeypadSubtract;
		case SDL_SCANCODE_KP_PLUS: return ImGuiKey_KeypadAdd;
		case SDL_SCANCODE_KP_ENTER: return ImGuiKey_KeypadEnter;
		case SDL_SCANCODE_KP_EQUALS: return ImGuiKey_KeypadEqual;
		default: break;
	}

	switch (keycode) {
		case SDLK_TAB: return ImGuiKey_Tab;
		case SDLK_LEFT: return ImGuiKey_LeftArrow;
		case SDLK_RIGHT: return ImGuiKey_RightArrow;
		case SDLK_UP: return ImGuiKey_UpArrow;
		case SDLK_DOWN: return ImGuiKey_DownArrow;
		case SDLK_PAGEUP: return ImGuiKey_PageUp;
		case SDLK_PAGEDOWN: return ImGuiKey_PageDown;
		case SDLK_HOME: return ImGuiKey_Home;
		case SDLK_END: return ImGuiKey_End;
		case SDLK_INSERT: return ImGuiKey_Insert;
		case SDLK_DELETE: return ImGuiKey_Delete;
		case SDLK_BACKSPACE: return ImGuiKey_Backspace;
		case SDLK_SPACE: return ImGuiKey_Space;
		case SDLK_RETURN: return ImGuiKey_Enter;
		case SDLK_ESCAPE: return ImGuiKey_Escape;
		//case SDLK_APOSTROPHE: return ImGuiKey_Apostrophe;
		case SDLK_COMMA: return ImGuiKey_Comma;
		//case SDLK_MINUS: return ImGuiKey_Minus;
		case SDLK_PERIOD: return ImGuiKey_Period;
		//case SDLK_SLASH: return ImGuiKey_Slash;
		case SDLK_SEMICOLON: return ImGuiKey_Semicolon;
		//case SDLK_EQUALS: return ImGuiKey_Equal;
		//case SDLK_LEFTBRACKET: return ImGuiKey_LeftBracket;
		//case SDLK_BACKSLASH: return ImGuiKey_Backslash;
		//case SDLK_RIGHTBRACKET: return ImGuiKey_RightBracket;
		//case SDLK_GRAVE: return ImGuiKey_GraveAccent;
		case SDLK_CAPSLOCK: return ImGuiKey_CapsLock;
		case SDLK_SCROLLLOCK: return ImGuiKey_ScrollLock;
		case SDLK_NUMLOCKCLEAR: return ImGuiKey_NumLock;
		case SDLK_PRINTSCREEN: return ImGuiKey_PrintScreen;
		case SDLK_PAUSE: return ImGuiKey_Pause;
		case SDLK_LCTRL: return ImGuiKey_LeftCtrl;
		case SDLK_LSHIFT: return ImGuiKey_LeftShift;
		case SDLK_LALT: return ImGuiKey_LeftAlt;
		case SDLK_LGUI: return ImGuiKey_LeftSuper;
		case SDLK_RCTRL: return ImGuiKey_RightCtrl;
		case SDLK_RSHIFT: return ImGuiKey_RightShift;
		case SDLK_RALT: return ImGuiKey_RightAlt;
		case SDLK_RGUI: return ImGuiKey_RightSuper;
		case SDLK_APPLICATION: return ImGuiKey_Menu;
		case SDLK_0: return ImGuiKey_0;
		case SDLK_1: return ImGuiKey_1;
		case SDLK_2: return ImGuiKey_2;
		case SDLK_3: return ImGuiKey_3;
		case SDLK_4: return ImGuiKey_4;
		case SDLK_5: return ImGuiKey_5;
		case SDLK_6: return ImGuiKey_6;
		case SDLK_7: return ImGuiKey_7;
		case SDLK_8: return ImGuiKey_8;
		case SDLK_9: return ImGuiKey_9;
		case SDLK_A: return ImGuiKey_A;
		case SDLK_B: return ImGuiKey_B;
		case SDLK_C: return ImGuiKey_C;
		case SDLK_D: return ImGuiKey_D;
		case SDLK_E: return ImGuiKey_E;
		case SDLK_F: return ImGuiKey_F;
		case SDLK_G: return ImGuiKey_G;
		case SDLK_H: return ImGuiKey_H;
		case SDLK_I: return ImGuiKey_I;
		case SDLK_J: return ImGuiKey_J;
		case SDLK_K: return ImGuiKey_K;
		case SDLK_L: return ImGuiKey_L;
		case SDLK_M: return ImGuiKey_M;
		case SDLK_N: return ImGuiKey_N;
		case SDLK_O: return ImGuiKey_O;
		case SDLK_P: return ImGuiKey_P;
		case SDLK_Q: return ImGuiKey_Q;
		case SDLK_R: return ImGuiKey_R;
		case SDLK_S: return ImGuiKey_S;
		case SDLK_T: return ImGuiKey_T;
		case SDLK_U: return ImGuiKey_U;
		case SDLK_V: return ImGuiKey_V;
		case SDLK_W: return ImGuiKey_W;
		case SDLK_X: return ImGuiKey_X;
		case SDLK_Y: return ImGuiKey_Y;
		case SDLK_Z: return ImGuiKey_Z;
		case SDLK_F1: return ImGuiKey_F1;
		case SDLK_F2: return ImGuiKey_F2;
		case SDLK_F3: return ImGuiKey_F3;
		case SDLK_F4: return ImGuiKey_F4;
		case SDLK_F5: return ImGuiKey_F5;
		case SDLK_F6: return ImGuiKey_F6;
		case SDLK_F7: return ImGuiKey_F7;
		case SDLK_F8: return ImGuiKey_F8;
		case SDLK_F9: return ImGuiKey_F9;
		case SDLK_F10: return ImGuiKey_F10;
		case SDLK_F11: return ImGuiKey_F11;
		case SDLK_F12: return ImGuiKey_F12;
		case SDLK_F13: return ImGuiKey_F13;
		case SDLK_F14: return ImGuiKey_F14;
		case SDLK_F15: return ImGuiKey_F15;
		case SDLK_F16: return ImGuiKey_F16;
		case SDLK_F17: return ImGuiKey_F17;
		case SDLK_F18: return ImGuiKey_F18;
		case SDLK_F19: return ImGuiKey_F19;
		case SDLK_F20: return ImGuiKey_F20;
		case SDLK_F21: return ImGuiKey_F21;
		case SDLK_F22: return ImGuiKey_F22;
		case SDLK_F23: return ImGuiKey_F23;
		case SDLK_F24: return ImGuiKey_F24;
		case SDLK_AC_BACK: return ImGuiKey_AppBack;
		case SDLK_AC_FORWARD: return ImGuiKey_AppForward;
		default: break;
	}

	// Fallback to scancode
	switch (scancode) {
		case SDL_SCANCODE_GRAVE: return ImGuiKey_GraveAccent;
		case SDL_SCANCODE_MINUS: return ImGuiKey_Minus;
		case SDL_SCANCODE_EQUALS: return ImGuiKey_Equal;
		case SDL_SCANCODE_LEFTBRACKET: return ImGuiKey_LeftBracket;
		case SDL_SCANCODE_RIGHTBRACKET: return ImGuiKey_RightBracket;
		case SDL_SCANCODE_BACKSLASH: return ImGuiKey_Backslash;
		case SDL_SCANCODE_SEMICOLON: return ImGuiKey_Semicolon;
		case SDL_SCANCODE_APOSTROPHE: return ImGuiKey_Apostrophe;
		case SDL_SCANCODE_COMMA: return ImGuiKey_Comma;
		case SDL_SCANCODE_PERIOD: return ImGuiKey_Period;
		case SDL_SCANCODE_SLASH: return ImGuiKey_Slash;
		default: break;
	}

	return ImGuiKey_None;
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
	io.BackendRendererName = "SDL3/BGFX";

	// connect Dear ImGui to SDL3 window
	io.DisplaySize = ImVec2(static_cast<float>(width), static_cast<float>(height));
	io.DeltaTime = 1.0f / 60.0f;

	// windows can only be dragged using the title bar
	io.ConfigWindowsMoveFromTitleBarOnly = true;

	// setup main viewport
	ImGui::GetMainViewport()->PlatformHandleRaw = nativeWindowHandle;

	// setup clipboard functions
	ImGuiPlatformIO& pio = ImGui::GetPlatformIO();
	pio.Platform_ClipboardUserData = this;
	pio.Platform_ImeUserData = this;

	pio.Platform_SetClipboardTextFn = [](ImGuiContext* /* ctx */, const char* text) {
		SDL_SetClipboardText(text);
	};

	pio.Platform_GetClipboardTextFn = [](ImGuiContext* /* ctx */) {
		OtFramework* framework = (OtFramework*) ImGui::GetPlatformIO().Platform_ClipboardUserData;
		framework->clipboardText = SDL_GetClipboardText();
		return framework->clipboardText.c_str();
	};

	pio.Platform_SetImeDataFn = [](ImGuiContext* /* ctx */, ImGuiViewport* /* viewport */, ImGuiPlatformImeData* data) {
		OtFramework* framework = (OtFramework*) ImGui::GetPlatformIO().Platform_ImeUserData;

		if ((!data->WantVisible || framework->imeWindow != framework->window) && framework->imeWindow != nullptr) {
			SDL_StopTextInput(framework->window);
			framework->imeWindow = nullptr;
		}

		if (data->WantVisible) {
			SDL_Rect rect;
			rect.x = static_cast<int>(data->InputPos.x);
			rect.y = static_cast<int>(data->InputPos.y);
			rect.w = 1;
			rect.h = static_cast<int>(data->InputLineHeight);

			SDL_SetTextInputArea(framework->window, &rect, 0);
			SDL_StartTextInput(framework->window);
			framework->imeWindow = framework->window;
		}
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
//	OtFramework::eventIMGUI
//

void OtFramework::eventIMGUI(SDL_Event& event) {
	ImGuiIO& io = ImGui::GetIO();

	switch (event.type) {
		case SDL_EVENT_MOUSE_MOTION: {
			io.AddMouseSourceEvent(event.wheel.which == SDL_TOUCH_MOUSEID ? ImGuiMouseSource_TouchScreen : ImGuiMouseSource_Mouse);
			io.AddMousePosEvent(static_cast<float>(event.motion.x), static_cast<float>(event.motion.y));
		break;
		}

		case SDL_EVENT_MOUSE_WHEEL: {
			io.AddMouseSourceEvent(event.wheel.which == SDL_TOUCH_MOUSEID ? ImGuiMouseSource_TouchScreen : ImGuiMouseSource_Mouse);
			io.AddMouseWheelEvent(static_cast<float>(-event.wheel.x), static_cast<float>(event.wheel.y));
		break;
		}

		case SDL_EVENT_MOUSE_BUTTON_DOWN:
		case SDL_EVENT_MOUSE_BUTTON_UP: {
			int button = -1;
			if (event.button.button == SDL_BUTTON_LEFT) { button = 0; }
			if (event.button.button == SDL_BUTTON_RIGHT) { button = 1; }
			if (event.button.button == SDL_BUTTON_MIDDLE) { button = 2; }
			if (event.button.button == SDL_BUTTON_X1) { button = 3; }
			if (event.button.button == SDL_BUTTON_X2) { button = 4; }

			if (button != -1) {
				io.AddMouseSourceEvent(event.button.which == SDL_TOUCH_MOUSEID ? ImGuiMouseSource_TouchScreen : ImGuiMouseSource_Mouse);
				io.AddMouseButtonEvent(button, event.type == SDL_EVENT_MOUSE_BUTTON_DOWN);
			}

			break;
		}

		case SDL_EVENT_TEXT_INPUT:
			io.AddInputCharactersUTF8(event.text.text);
			break;

		case SDL_EVENT_KEY_DOWN:
		case SDL_EVENT_KEY_UP: {
			if (event.key.key == SDLK_F16 && event.type == SDL_EVENT_KEY_DOWN) {
				profiler = !profiler;

			} else if (event.key.key == SDLK_F17 && event.type == SDL_EVENT_KEY_DOWN) {
				metrics = !metrics;

			} else if (event.key.key == SDLK_F18 && event.type == SDL_EVENT_KEY_DOWN) {
				demo = !demo;

			} else {
				io.AddKeyEvent(ImGuiMod_Ctrl, (event.key.mod & SDL_KMOD_CTRL) != 0);
				io.AddKeyEvent(ImGuiMod_Shift, (event.key.mod & SDL_KMOD_SHIFT) != 0);
				io.AddKeyEvent(ImGuiMod_Alt, (event.key.mod & SDL_KMOD_ALT) != 0);
				io.AddKeyEvent(ImGuiMod_Super, (event.key.mod & SDL_KMOD_GUI) != 0);
				io.AddKeyEvent(toImGuiKey(event.key.key, event.key.scancode), event.type == SDL_EVENT_KEY_DOWN);
			}

			break;
		}

		case SDL_EVENT_WINDOW_FOCUS_GAINED:
		case SDL_EVENT_WINDOW_FOCUS_LOST:
			io.AddFocusEvent(event.type == SDL_EVENT_WINDOW_FOCUS_GAINED);
			break;
	}
}


//
//	OtFramework::startFrameIMGUI
//

void OtFramework::startFrameIMGUI() {
	// update Dear ImGui state
	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2(static_cast<float>(width), static_cast<float>(height));
	io.DeltaTime = loopDuration / 1000.0f;

	// update cursor
	if (!(io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)) {
		ImGuiMouseCursor cursor = ImGui::GetMouseCursor();

		if (io.MouseDrawCursor || cursor == ImGuiMouseCursor_None) {
			// hide OS mouse cursor if Dear ImGui is drawing it or if it wants no cursor
			SDL_HideCursor();

		} else {
			// update cursor if required
			if (cursor != lastCursor) {
				lastCursor = cursors[cursor] ? cursor : ImGuiMouseCursor_Arrow;
				SDL_SetCursor(cursors[lastCursor]);
			}

			// Show OS mouse cursor
			SDL_ShowCursor();
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
//	OtFramework::endFrameIMGUI
//

void OtFramework::endFrameIMGUI() {
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
