//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <chrono>
#include <string>

#include "SDL3/SDL.h"
#include "bgfx/bgfx.h"
#include "imgui.h"

#include "OtFrameworkApp.h"
#include "OtSampler.h"
#include "OtShaderProgram.h"
#include "OtTexture.h"


//
//	OtFramework
//

class OtFramework {
public:
	// run the UI framework
	void run(OtFrameworkApp* app);

	// stop the UI framework
	void stop();

	// see if we can safely quit the program
	bool canClose();
	bool canQuit();

	// set anti-aliasing
	void setAntiAliasing(int level);

	// open specified URL in browser
	static void openURL(const std::string& url);

private:
#if __APPLE__
	// put the right app name in the menu and adjust windows menu
	void fixMenus();
#endif

	// render a debugging profiler
	void renderProfiler();

	// initialize, run and terminate libraries
	void initSDL();
	void eventsSDL();
	void endSDL();

	void initBGFX();
	void startFrameBGFX();
	void endFrameBGFX();
	void endBGFX();

	void initIMGUI();
	void eventIMGUI(SDL_Event& event);
	void startFrameIMGUI();
	void updateTextureIMGUI(ImTextureData* texture);
	void endFrameIMGUI();
	void endIMGUI();

	// main window
	SDL_Window* window = nullptr;
	void* nativeWindowHandle = nullptr;
	void* nativeDisplayType = nullptr;
	int width;
	int height;

	// Input Method Editor (IME) support
	SDL_Window* imeWindow = nullptr;

	// time tracking
	std::chrono::time_point<std::chrono::high_resolution_clock> lastTime;
	std::chrono::time_point<std::chrono::high_resolution_clock> loopTime;
	float loopDuration;
	float cpuTime;

	// support Dear ImGui
	bgfx::VertexLayout imguiVertexLayout;
	OtSampler imguiTextureSampler{"s_imguiTexture", OtSampler::pointSampling | OtSampler::clampSampling};
	OtShaderProgram imguiShaderProgram{"OtImGuiVS", "OtImGuiFS"};

	// cursors
	SDL_Cursor* cursors[ImGuiMouseCursor_COUNT];
	ImGuiMouseCursor lastCursor = ImGuiMouseCursor_None;

	// clipboard
	std::string clipboardText;

	// anti-aliasing setting
	int antiAliasing = 0;

	// show/hide profiler/demo
	bool profiler = false;
	bool metrics = false;
	bool demo = false;

	// are we running?
	bool running;

	// our app
	OtFrameworkApp* app;
};
