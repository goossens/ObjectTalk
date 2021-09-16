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
#include <GLFW/glfw3.h>

#if __APPLE__
#define GLFW_EXPOSE_NATIVE_COCOA

#elif defined(_WIN32)
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL

#else
#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_GLX
#endif

#include <GLFW/glfw3native.h>

#include <bgfx/platform.h>

#include "application.h"
#include "theme.h"


//
//	OtApplicationClass::initBGFX
//

void OtApplicationClass::initBGFX() {
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

	init.resolution.width  = width;
	init.resolution.height = height;
	init.resolution.reset  = BGFX_RESET_VSYNC;
	init.debug = false;

	bgfx::init(init);

	bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x000000ff, 1.0f, 0);
	bgfx::setViewRect(0, 0, 0, width, height);
}


//
//	OtApplicationClass::frameBGFX
//

void OtApplicationClass::frameBGFX() {
	bgfx::reset(width, height, BGFX_RESET_VSYNC | BGFX_RESET_MSAA_X8);
	bgfx::setViewRect(0, 0, 0, width, height);
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
