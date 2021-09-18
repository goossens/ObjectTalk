//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <mutex>
#include <vector>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <bgfx/bgfx.h>

#include <imgui.h>

#include "gui.h"
#include "animation.h"
#include "simulation.h"
#include "screen.h"


//
//	OtApplicationClass
//

class OtApplicationClass;
typedef std::shared_ptr<OtApplicationClass> OtApplication;

class OtApplicationClass : public OtGuiClass {
public:
	// run the application
	void run(const std::string& name);
	void runThread2();

	// mouse and keyboard events
	void onMouseButton(int button, int action, int mods, double xpos, double ypos);
	void onMouseMove(int button, double xpos, double ypos);
	void onKey(int key, int scancode, int action, int mods);
	void onChar(unsigned int codepoint);

	// add an animation
	OtObject animation();

	// add a simulation
	OtObject addSimulation(OtObject simulation);

	// set anti-aliasing
	OtObject setAntiAliasing(int level);

	// (de)activate profiler
	OtObject setProfiler(bool profilerState);

	// get frame dimensions
	static int getWidth() { return width; }
	static int getHeight() { return height; }

	// get system time in seconds since the epoch
	static double getTime();

	// get the current frame number
	static size_t getFrameNumber() { return frameNumber; }

	// get the current frame rate
	static double getFrameRate() { return 1.0 / loopDuration; }

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtApplication create();

private:
#if __APPLE__
	// put the right app name in the menus
	void fixMenuLabels(const std::string& name);

	// create a Metal layer in the narive window
	void createMetalLayer();
#endif

	// render a debugging profiler
	void renderProfiler();

	// initialize, run and terminate libraries
	void initGLFW(const std::string& name);
	bool runningGLFW();
	void eventsGLFW();
	void endGLFW();

	void initBGFX();
	void frameBGFX();
	void renderBGFX();
	void endBGFX();

	void initIMGUI();
	void frameIMGUI();
	void renderIMGUI();
	void endIMGUI();

	// main window
	GLFWwindow* window;
	void* nativeDisplayHandle;
	void* nativeDisplayType = nullptr;
	static int width;
	static int height;

	// mouse state
	int mouseButton;
	int mouseAction;
	int mouseMods;
	double mouseX;
	double mouseY;

	// keyboard state
	bool keyboardState[512] = {0};
	int keyboardKey;
	int keyboardScancode;
	int keyboardAction;
	int keyboardMods;
	int keyboardCodepoint;

	// time tracking
	static size_t frameNumber;
	static double lastTime;
	static double loopTime;
	static double loopDuration;
	static double cpuDuration;
	static double gpuDuration;

	// animations
	std::vector<OtAnimation> animations;

	// simulations
	std::vector<OtSimulation> simulations;

	// to render IM3D
	bgfx::VertexLayout  im3dVertexLayout;
	bgfx::ProgramHandle im3dProgram = BGFX_INVALID_HANDLE;

	// to render IMGUI
	bgfx::VertexLayout  imguiVertexLayout;
	bgfx::TextureHandle imguiFontTexture = BGFX_INVALID_HANDLE;
	bgfx::UniformHandle imguiFontUniform = BGFX_INVALID_HANDLE;
	bgfx::ProgramHandle imguiProgram = BGFX_INVALID_HANDLE;

	// top level widget
	OtScreen screen;

	// anti-aliasing setting
	int antiAliasing = 0;

	// show/hide profiler
	bool profiler = false;

	// is secondary thread running?
	bool running;

	// bridge between threads
	std::mutex lock;
	std::vector<std::string> events;

	void pushEvent(const std::string& event);
	bool hasEvents();
	std::string popEvent();
	void handleEvent();

	// event tracking
	bool clickEvent;
	bool moveEvent;
	bool keyEvent;
	bool charEvent;
};
