//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cstring>

#define GLFW_INCLUDE_NONE
#import <GLFW/glfw3.h>

#define GLFW_EXPOSE_NATIVE_COCOA
#import <GLFW/glfw3native.h>

#include <bgfx/platform.h>

#include "application.h"


//
//	OtApplicationClass::initBGFX
//

void OtApplicationClass::initBGFX() {
	// single threaded mode
	bgfx::renderFrame();

	// initialize bgfx
	bgfx::Init init;

#if __APPLE__
	init.type = bgfx::RendererType::Metal;
	init.platformData.nwh = glfwGetCocoaWindow(window);

#elif defined(_WIN32)
	init.type = bgfx::RendererType::Direct3D12;
	init.platformData.nwh = glfwGetWin32Window(window);

#else
	init.type = bgfx::RendererType::OpenGLES;
	init.platformData.nwh = (void*) glfwGetX11Window(window);
	init.platformData.ndt = glfwGetX11Display();
#endif

	init.resolution.width  = OtTheme::width;
	init.resolution.height = OtTheme::height;
	init.resolution.reset  = BGFX_RESET_VSYNC;
	init.debug = false;

	bgfx::init(init);

	bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x000000ff, 1.0f, 0);
	bgfx::setViewRect(0, 0, 0, OtTheme::width, OtTheme::height);
}


//
//	OtApplicationClass::frameBGFX
//

void OtApplicationClass::frameBGFX() {
	bgfx::reset(OtTheme::width, OtTheme::height, BGFX_RESET_VSYNC);
	bgfx::setViewRect(0, 0, 0, OtTheme::width, OtTheme::height);
	bgfx::touch(0);
}


//
//	OtApplicationClass::renderBGFX
//

void OtApplicationClass::renderBGFX() {
	bgfx::frame();
}


//
//	OtApplicationClass::endBGFX
//

void OtApplicationClass::endBGFX() {
	bgfx::shutdown();
}
