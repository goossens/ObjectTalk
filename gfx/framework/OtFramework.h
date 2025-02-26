//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <array>
#include <string>
#include <vector>

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "bgfx/bgfx.h"
#include "bimg/bimg.h"
#include "bgfx/embedded_shader.h"
#include "imgui.h"

#include "OtClipboard.h"
#include "OtFrameworkApp.h"
#include "OtFrameworkEvents.h"
#include "OtSampler.h"
#include "OtShaderProgram.h"
#include "OtTexture.h"


//
//	OtFramework
//

class OtFramework {
public:
	// initialize UI framework
	void initialize();

	// terminate UI framework
	void terminate();

	// run the UI framework
	void run(OtFrameworkApp* app);

	// stop the UI framework
	void stop();

	// see if we can safely quit the program
	bool canQuit();

	// set anti-aliasing
	void setAntiAliasing(int level);

	// open specified URL in browser
	static void openURL(const std::string& url);

	// get list of file to open on startup
	inline static std::vector<std::string>& getStartupFiles() { return filesToOpen; }

#if __APPLE__
	// add file to open at startup
	static void addStartupFile(const char* filename);
#endif

private:
#if __APPLE__
	// put the right app name in the menu
	void fixMenuLabels();

	// create a Metal layer in the native window
	// this is required since we are running in a multithreaded configuration
	// BGFX would create the metal layer automatically but in the wrong thread
	// MacOS only allows certain API calls from the main thread
	void createMetalLayer();
#endif

	// the second thread runs the application logic as well as the asynchronous libuv events
	void runThread2();

	// render a debugging profiler
	void renderProfiler();

	// initialize, run and terminate libraries
	void initGLFW();
	bool runningGLFW();
	void eventsGLFW();
	void stopGLFW();
	void endGLFW();

	void initBGFX();
	void frameBGFX();
	void renderBGFX();
	void endBGFX();

	void initIMGUI();
	void frameIMGUI();
	void renderIMGUI();
	void endIMGUI();

	// files to open on startup
	static std::vector<std::string> filesToOpen;

	// main window
	GLFWwindow* window;
	void* nativeDisplayHandle;
	void* nativeDisplayType = nullptr;
	int width;
	int height;

	// time tracking
	int64_t lastTime;
	int64_t loopTime;
	float loopDuration;
	float cpuTime;

	// support Dear ImGui
	bgfx::VertexLayout imguiVertexLayout;
	OtTexture imguiFontTexture;
	OtSampler imguiFontSampler{"s_imguiFontAtlas"};
	OtShaderProgram imguiShaderProgram{"OtImGuiVS", "OtImGuiFS"};

	// cursors
	GLFWcursor* cursors[ImGuiMouseCursor_COUNT];
	ImGuiMouseCursor cursor;
	bool setCursor = false;

	// anti-aliasing setting
	int antiAliasing = 0;

	// show/hide profiler/demo
	bool profiler = false;
	bool metrics = false;
	bool demo = false;

	// are we running?
	bool running;

	// bridges between threads
	OtFwEventQueue eventQueue;
	OtClipboard clipboard;
	std::string clipboardText;

#if __APPLE__
	// for right/middle click simulation (using ctrl-click)
	bool inRightClick = false;
	bool inMiddleClick = false;
#endif

	// track gamepad status
	struct Gamepad {
		int buttons[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	};

	std::array<Gamepad, 4> gamepads;

	// our app
	OtFrameworkApp* app;
};
