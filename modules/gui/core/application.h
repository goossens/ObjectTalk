//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#define GLFW_INCLUDE_NONE
#import <GLFW/glfw3.h>

#include <bgfx/bgfx.h>

#include <imgui.h>

#include "gui.h"
#include "screen.h"
#include "theme.h"


//
//	OtApplicationClass
//

class OtApplicationClass;
typedef std::shared_ptr<OtApplicationClass> OtApplication;

class OtApplicationClass : public OtGuiClass {
public:
	// run the application
	void run(const std::string& name);

	// mouse and keyboard events
	void onMouseButton(int button, int action, int mods, double xpos, double ypos);
	void onMouseMove(int button, double xpos, double ypos);
	void onKey(int key, int scancode, int action, int mods);
	void onChar(unsigned int codepoint);

	// animate a parameter
	static void animate(double* value, double delay, double length, double start, double end);

	// get system time in milliseconds since the epoch
	static double getTime();

	// get the current framerate
	static double getFrameRate() { return 1.0 / loopDuration; }

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtApplication create();

private:
#if __APPLE__
	// put the right app name in the menus
	void fixMenuLabels(const std::string& name);
#endif

	// render a single frame
	void render();

	// initialize, run and terminate libraries
	void initGLFW(const std::string& name);
	void frameGLFW();
	void eventsGLFW();
	void endGLFW();

	void initBGFX();
	void frameBGFX();
	void renderBGFX();
	void endBGFX();

	void initIMGUI();
	void frameIMGUI();
	bool mouseIMGUI();
	bool keyboardIMGUI();
	void renderIMGUI();
	void endIMGUI();

	// main window
	GLFWwindow* window;

	// time tracking
	static double lastTime;
	static double loopTime;
	static double loopDuration;

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
};
