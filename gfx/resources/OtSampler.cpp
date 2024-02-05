//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtLog.h"

#include "OtCubeMap.h"
#include "OtSampler.h"
#include "OtTexture.h"


//
//	OtSampler::initialize
//

void OtSampler::initialize(const char* n, uint32_t f) {
	if (n != name || flags != f) {
		if (isValid()) {
			uniform.clear();
		}

		name = n;
		flags = f;
	}
}


//
// OtSampler::clear
//

void OtSampler::clear() {
	flags = defaultSampling;
	name.clear();
	uniform.clear();
}


//
//	OtSampler::submit
//

void OtSampler::submit(int unit, const char* n) {
	// initialize sampler if new name is provided
	if (n) {
		initialize(n);
	}

	// generate resource (if required)
	if (!isValid()) {
		createUniform();
	}

	// submit dummy texture
	OtTexture dummy;
	bgfx::setTexture(unit, uniform.getHandle(), dummy.getHandle(), flags);
}

void OtSampler::submit(int unit, OtTexture& texture, const char* name) {
	// see if texture is valid/ready
	if (texture.isValid()) {
		// initialize sampler if new name is provided
		if (name) {
			initialize(name);
		}

		// generate resource (if required)
		if (!isValid()) {
			createUniform();
		}

		// submit texture
		bgfx::setTexture(unit, uniform.getHandle(), texture.getHandle(), flags);

	} else {
		// submit dummy texture
		submit(unit, name);
	}
}

void OtSampler::submit(int unit, bgfx::TextureHandle texture, const char* name) {
	// see if texture is valid/ready
	if (bgfx::isValid(texture)) {
		// initialize sampler if new name is provided
		if (name) {
			initialize(name);
		}

		// generate resource (if required)
		if (!isValid()) {
			createUniform();
		}

		// submit texture
		bgfx::setTexture(unit, uniform.getHandle(), texture, flags);

	} else {
		submit(unit, name);
	}
}

void OtSampler::submit(int unit, OtCubeMap& cubemap, const char* name) {
	// initialize sampler if new name is provided
	if (name) {
		initialize(name);
	}

	// generate resource (if required)
	if (!isValid()) {
		createUniform();
	}

	// submit texture
	bgfx::setTexture(unit, uniform.getHandle(), cubemap.getHandle(), flags);
}


//
//	OtSampler::createUniform
//

void OtSampler::createUniform() {
	if (!name.size()) {
		OtLogFatal("internal error: sampler not initialized before submission");
	}

	uniform = bgfx::createUniform(name.c_str(), bgfx::UniformType::Sampler);
}
