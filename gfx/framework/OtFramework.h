//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
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

#include "OtFrameworkApp.h"


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

	void initIMGUI();
	void eventIMGUI(SDL_Event& event);
	void startFrameIMGUI();
	void endFrameIMGUI();
	void endIMGUI();

	// window information
	SDL_Window* window;

	// time tracking
	std::chrono::time_point<std::chrono::high_resolution_clock> lastTime;
	std::chrono::time_point<std::chrono::high_resolution_clock> loopTime;
	float loopDuration;
	float cpuTime;
	float gpuTime;
	float gpuWaitTime;

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
