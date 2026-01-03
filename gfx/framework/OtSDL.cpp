//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cstring>

#include "OtLog.h"

#include "OtFramework.h"
#include "OtGpu.h"


//
//	OtFramework::initSDL
//

void OtFramework::initSDL() {
	// initialize SDL library
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		OtLogFatal("Can't initialize SDL library: {}", SDL_GetError());
	}

	// determine best default window size
	int width;
	int height;
	int numberOfDisplays;
	SDL_DisplayID* displays = SDL_GetDisplays(&numberOfDisplays);

	if (numberOfDisplays == 0) {
		OtLogFatal("SDL library can't find any displays");
	}

	SDL_Rect rect;
	SDL_GetDisplayBounds(displays[0], &rect);
	SDL_free(displays);

	if (rect.w >= 1600 && rect.h >= 900) {
		width = 1600;
		height = 900;

	} else if (rect.w >= 1440 && rect.h >= 810) {
		width = 1440;
		height = 810;

	} else if (rect.w >= 1280 && rect.h >= 720) {
		width = 1280;
		height = 720;

	} else if (rect.w >= 1024 && rect.h >= 576) {
		width = 1024;
		height = 576;

	} else if (rect.w >= 800 && rect.h >= 450) {
		width = 800;
		height = 450;

	} else {
		width = 640;
		height = 360;
	}

	// create a new window
	window = SDL_CreateWindow("ObjectTalk", width, height, SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);

	if (!window) {
		OtLogFatal("Error in SDL_CreateWindow: {}", SDL_GetError());
	}

	if (!SDL_SetWindowAspectRatio(window, 16.0f / 9.0f, 16.0f / 9.0f)) {
		OtLogFatal("Error in SDL_SetWindowAspectRatio: {}", SDL_GetError());
	}

	SDL_SetHint(SDL_HINT_QUIT_ON_LAST_WINDOW_CLOSE, "0");

	// create GPU device
	OtGpu::instance().init(window, width, height);

#if __APPLE__
	fixMenus();
#endif
}


//
//	OtFramework::eventsSDL
//

void OtFramework::eventsSDL() {
	// process available events
	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		eventIMGUI(event);

		switch (event.type) {
			case SDL_EVENT_WINDOW_RESIZED:
				OtGpu::instance().setWindowSize(event.window.data1, event.window.data2);
				break;

			case SDL_EVENT_QUIT:
				if (canQuit()) {
					stop();
				}

				break;

			case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
				if (canQuit()) {
					stop();
				}

				break;

			case SDL_EVENT_KEY_DOWN: {
#if __APPLE__
				static SDL_Keymod modifier = SDL_KMOD_GUI;
#else
				static SDL_Keymod modifier = SDL_KMOD_CTRL;
#endif

				if ((event.key.mod & modifier) && event.key.key == SDLK_Q) {
					if (canQuit()) {
						stop();
					}

				} else if ((event.key.mod & modifier) && event.key.key == SDLK_W) {
					if (canClose()) {
						stop();
					}
				}

				break;
			}
		}
	}
}


//
//	OtFramework::endSDL
//

void OtFramework::endSDL() {
	// cleanup
	OtGpu::instance().release();
	SDL_DestroyWindow(window);
	SDL_Quit();
}


//
//	OtFramework::openURL
//

void OtFramework::openURL(const std::string& url) {
	SDL_OpenURL(url.c_str());
}
