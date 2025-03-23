//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtFunction.h"
#include "OtInteger.h"
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

	// determine window properties
	SDL_PropertiesID props = SDL_CreateProperties();

	if (!props) {
		OtLogFatal("SDL library can't create properties: {}", SDL_GetError());
	}

	SDL_SetStringProperty(props, SDL_PROP_WINDOW_CREATE_TITLE_STRING, "ObjectTalk");
	SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_RESIZABLE_BOOLEAN, true);
	SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, width);
	SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, height);
	SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_HIGH_PIXEL_DENSITY_BOOLEAN, true);

#if OT_GPU_METAL
	SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_METAL_BOOLEAN, true);

#elif OT_GPU_VULKAN
	SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_VULKAN_BOOLEAN, true);
#endif

	// create a new window
	window = SDL_CreateWindowWithProperties(props);

	if (!window) {
		OtLogFatal("SDL library can't create window: {}", SDL_GetError());
	}

	SDL_DestroyProperties(props);
	SDL_SetWindowAspectRatio(window, 16.0f / 9.0f, 16.0f / 9.0f);

	// get native handles
#if __APPLE__
	nativeWindowHandle = static_cast<void*>(SDL_GetPointerProperty(
		SDL_GetWindowProperties(window),
		SDL_PROP_WINDOW_COCOA_WINDOW_POINTER,
		nullptr));

	createMetalLayer();

#elif _WIN32
	nativeWindowHandle = static_cast<void*>(SDL_GetPointerProperty(
		SDL_GetWindowProperties(window),
		SDL_PROP_WINDOW_WIN32_HWND_POINTER,
		nullptr));

#else
	if (SDL_strcmp(SDL_GetCurrentVideoDriver(), "x11") == 0) {
		nativeDisplayType = static_cast<void*>(SDL_GetPointerProperty(
			SDL_GetWindowProperties(window),
			SDL_PROP_WINDOW_X11_DISPLAY_POINTER,
			nullptr));

		nativeWindowHandle = (void*) SDL_GetNumberProperty(
			SDL_GetWindowProperties(window),
			SDL_PROP_WINDOW_X11_WINDOW_NUMBER,
			0);

		if (!nativeDisplayType) {
			OtLogFatal("X11 display not configured");
		}

		if (!nativeWindowHandle){
			OtLogFatal("X11 window not configured");
		}

	} else if (SDL_strcmp(SDL_GetCurrentVideoDriver(), "wayland") == 0) {
		nativeDisplayType = static_cast<void*>(SDL_GetPointerProperty(
			SDL_GetWindowProperties(window),
			SDL_PROP_WINDOW_WAYLAND_DISPLAY_POINTER,
			nullptr));

		nativeWindowHandle = (void*) SDL_GetPointerProperty(
			SDL_GetWindowProperties(window),
			SDL_PROP_WINDOW_WAYLAND_EGL_WINDOW_POINTER,
			nullptr);

		if (!nativeDisplayType) {
			OtLogFatal("Wayland display not configured");
		}

		if (!nativeWindowHandle){
			OtLogFatal("Wayland window not configured");
		}

	} else {
		OtLogFatal("Unknown video driver: {}", SDL_GetCurrentVideoDriver());
	}
#endif

#if __APPLE__
	fixMenus();
#endif

	// create cursors
	cursors[ImGuiMouseCursor_Arrow] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT);
	cursors[ImGuiMouseCursor_TextInput] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_TEXT);
	cursors[ImGuiMouseCursor_ResizeAll] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_MOVE);
	cursors[ImGuiMouseCursor_ResizeNS] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NS_RESIZE);
	cursors[ImGuiMouseCursor_ResizeEW] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_EW_RESIZE);
	cursors[ImGuiMouseCursor_ResizeNESW] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NESW_RESIZE);
	cursors[ImGuiMouseCursor_ResizeNWSE] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NWSE_RESIZE);
	cursors[ImGuiMouseCursor_Hand] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_POINTER);
	cursors[ImGuiMouseCursor_NotAllowed] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NOT_ALLOWED);
}


//
//	OtFramework::eventsSDL
//

void OtFramework::eventsSDL() {
	// process available events
	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
				if (canQuit()) {
					stop();
				}

				break;

			case SDL_EVENT_WINDOW_RESIZED:
				width = event.window.data1;
				height = event.window.data2;
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
				}

				break;
			}
		}

		eventIMGUI(event);
	}
}


//
//	OtFramework::endSDL
//

void OtFramework::endSDL() {
	// free cursors
	for (ImGuiMouseCursor c = 0; c < ImGuiMouseCursor_COUNT; c++) {
		SDL_DestroyCursor(cursors[c]);
	}

	// terminate SDL
	SDL_DestroyWindow(window);
	SDL_Quit();
}


//
//	OtFramework::openURL
//

void OtFramework::openURL(const std::string& url) {
	SDL_OpenURL(url.c_str());
}
