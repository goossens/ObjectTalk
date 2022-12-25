//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtException.h"
#include "OtFormat.h"
#include "OtRegistry.h"

#include "OtFramework.h"
#include "OtShader.h"
#include "OtShaders.h"


//
//	OtShader::OtShader
//

OtShader::OtShader(const char* vertex, const char* fragment) {
	initialize(vertex, fragment);
}


//
//	OtShader::~OtShader
//

OtShader::~OtShader() {
	clear();
}


//
//	OtShader::initialize
//

void OtShader::initialize(const char* vertex, const char* fragment) {
	// initialize registry if required
	static bool initialized = false;
	static OtRegistry<bgfx::ProgramHandle> registry;

	if (!initialized) {
		OtFrameworkClass::instance()->atexit([] {
			registry.iterateValues([] (bgfx::ProgramHandle shader) {
				bgfx::destroy(shader);
			});

			registry.clear();
			initialized = false;
		});

		initialized = true;
	}

	// see if we already have this shader
	std::string index = OtFormat("%s, %s", vertex, fragment);

	if (registry.has(index)) {
		shader = registry.get(index);

	} else {
		bgfx::RendererType::Enum type = bgfx::getRendererType();

		shader = bgfx::createProgram(
		bgfx::createEmbeddedShader(embeddedShaders, type, vertex),
		bgfx::createEmbeddedShader(embeddedShaders, type, fragment),
		true);

		registry.set(index, shader);
	}
}


//
//	OtShader::clear
//

void OtShader::clear() {
	shader = BGFX_INVALID_HANDLE;
}


//
//	OtShader::submit
//

void OtShader::submit(bgfx::ViewId view) {
	if (!bgfx::isValid(shader)) {
		OtExcept("Internal error: Shader not initialized before submission");
	}

	bgfx::submit(view, shader);
}
