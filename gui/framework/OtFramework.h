//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <array>
#include <functional>
#include <unordered_map>
#include <vector>

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "bgfx/bgfx.h"
#include "bimg/bimg.h"
#include "bgfx/embedded_shader.h"

#include "OtException.h"
#include "OtObject.h"
#include "OtRegistry.h"
#include "OtSingleton.h"

#include "OtCustomer.h"
#include "OtFrameworkEvents.h"


//
//	OtFrameworkClass
//

class OtFrameworkClass;
typedef std::shared_ptr<OtFrameworkClass> OtFramework;

class OtFrameworkClass : public OtSingleton<OtFrameworkClass> {
public:
	// constructor
	OtFrameworkClass();

	// run the GUI framework
	void run();

	// stop the GUI framework because of an error
	void error(OtException e);

	// stop the GUI framework
	void stop();

	// manage customers
	void addCustomer(OtCustomer* customer);
	void removeCustomer(OtCustomer* customer);

	// set anti-aliasing
	void setAntiAliasing(int level);

	// get frame dimensions
	int getWidth() { return width; }
	int getHeight() { return height; }

	// get system time in seconds since application start
	float getTime();

	// get loop duration in milliseconds
	float getLoopDuration() { return loopDuration; }

	// get the current frame number
	size_t getFrameNumber() { return frameNumber; }

	// get the current frame rate
	float getFrameRate() { return 1000.0 / loopDuration; }

	// get the next available view ID
	bgfx::ViewId getNextViewID() { return nextViewID++; }

	// program/image/texture functions
	bgfx::ProgramHandle getProgram(const bgfx::EmbeddedShader* shaders, const char* vertex, const char* fragment);
	bgfx::TextureHandle getTexture(const std::string& file, bool mipmap = false, bimg::ImageContainer** image = nullptr);
	bimg::ImageContainer* getImage(const std::string& file, bool powerof2 = false, bool square = false);
	bgfx::TextureHandle getDummyTexture() { return dummyTexture; }

	// add enums to specified module
	void addEnumsGLFW(OtObject module);
	void addEnumsIMGUI(OtObject module);

	// register function to be called at exit
	void atexit(std::function<void(void)> callback) {
		atExitCallbacks.push_back(callback);
	}

private:
#if __APPLE__
	// put the right app name in the menu
	void fixMenuLabels();

	// create a Metal layer in the narive window
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
	void frameIMGUI(std::vector<OtFwEvent>& events);
	void renderIMGUI();
	void endIMGUI();

	// main window
	GLFWwindow* window;
	void* nativeDisplayHandle;
	void* nativeDisplayType = nullptr;
	int width = 1280;
	int height = 720;
	bgfx::ViewId nextViewID;

	// time tracking
	size_t frameNumber;
	int64_t startTime;
	int64_t lastTime;
	int64_t loopTime;
	float loopDuration;
	int64_t cpuDuration;
	int64_t gpuDuration;

	// program/image/texture registries
	OtRegistry<bgfx::ProgramHandle> programRegistry;
	OtRegistry<bgfx::TextureHandle> textureRegistry;
	OtRegistry<bimg::ImageContainer*> imageRegistry;
	std::unordered_map<uint16_t, bimg::ImageContainer*> textureImageMap;

	bimg::ImageContainer* dummyImage;
	bgfx::TextureHandle dummyTexture;

	// to render IMGUI
	bgfx::VertexLayout imguiVertexLayout;
	bgfx::TextureHandle imguiFontTexture = BGFX_INVALID_HANDLE;
	bgfx::UniformHandle imguiFontUniform = BGFX_INVALID_HANDLE;
	bgfx::ProgramHandle imguiProgram = BGFX_INVALID_HANDLE;

	// anti-aliasing setting
	int antiAliasing = 0;

	// show/hide profiler/demo
	bool profiler = false;
	bool demo = false;

	// are we running?
	bool running;

	// bridge between threads
	OtFwEventQueue eventQueue;

	// track keyboard modifier state
	int modifiers = 0;

	// track gamepad status
	struct Gamepad {
		int buttons[8] = {0};
	};

	std::array<Gamepad, 4> gamepads;

	// our customers
	std::vector<OtCustomer*> customers;

	// callbacks
	std::vector<std::function<void(void)>> atExitCallbacks;
};
