//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtException.h"
#include "OtRegistry.h"

#include "OtFramework.h"
#include "OtSampler.h"


//
//	OtSampler::OtSampler
//

OtSampler::OtSampler(const char *name) {
	initialize(name);
}


//
//	OtSampler::~OtSampler
//

OtSampler::~OtSampler() {
	clear();
}


//
//	OtSampler::initialize
//

void OtSampler::initialize(const char* name) {
	// initialize registry if required
	static bool initialized = false;
	static OtRegistry<bgfx::UniformHandle> registry;

	if (!initialized) {
		OtFrameworkClass::instance()->atexit([] {
			registry.iterateValues([] (bgfx::UniformHandle uniform) {
				bgfx::destroy(uniform);
			});

			registry.clear();
			initialized = false;
		});

		initialized = true;
	}

	// see if we already have this shader
	if (registry.has(name)) {
		uniform = registry.get(name);

	} else {
		uniform = bgfx::createUniform(name, bgfx::UniformType::Sampler);
		registry.set(name, uniform);
	}
}


//
//	OtSampler::clear
//

void OtSampler::clear() {
	uniform = BGFX_INVALID_HANDLE;
}


//
//	OtSampler::submit
//

void OtSampler::submit(int unit) {
	if (!bgfx::isValid(uniform)) {
		OtExcept("internal error: sampler not initialized");
	}

	OtTexture dummy;
	bgfx::setTexture(unit, uniform, dummy.getTextureHandle());
}

void OtSampler::submit(int unit, OtTexture &texture) {
	if (!bgfx::isValid(uniform)) {
		OtExcept("internal error: sampler not initialized");
	}

	bgfx::setTexture(unit, uniform, texture.getTextureHandle());
}

void OtSampler::submit(int unit, bgfx::TextureHandle texture) {
	if (!bgfx::isValid(uniform)) {
		OtExcept("internal error: sampler not initialized");
	}

	bgfx::setTexture(unit, uniform, texture);
}

void OtSampler::submit(int unit, OtCubeMap& cubemap) {
	if (!bgfx::isValid(uniform)) {
		OtExcept("internal error: sampler not initialized");
	}

	bgfx::setTexture(unit, uniform, cubemap.getTextureHandle());
}
