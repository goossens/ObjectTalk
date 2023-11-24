//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtLog.h"

#include "OtSampler.h"


//
//	OtSampler::OtSampler
//

OtSampler::OtSampler(const char* name, uint32_t f) : flags(f) {
	initialize(name);
}


//
//	OtSampler::initialize
//

void OtSampler::initialize(const char* name) {
	// create the uniform
	uniformName = name;
	uniform = bgfx::createUniform(name, bgfx::UniformType::Sampler);
}


//
//	OtSampler::submit
//

void OtSampler::submit(int unit, const char* name) {
	if (name && name != uniformName) {
		initialize(name);
	}

	if (!uniform.isValid()) {
		OtLogFatal("internal error: sampler not initialized");
	}

	OtTexture dummy;
	bgfx::setTexture(unit, uniform.getHandle(), dummy.getTextureHandle(), flags);
}

void OtSampler::submit(int unit, OtTexture& texture, const char* name) {
	if (texture.isValid()) {
		if (name && name != uniformName) {
			initialize(name);
		}

		if (!uniform.isValid()) {
			OtLogFatal("internal error: sampler not initialized");
		}

		bgfx::setTexture(unit, uniform.getHandle(), texture.getTextureHandle(), flags);

	} else {
		submit(unit, name);
	}
}

void OtSampler::submit(int unit, bgfx::TextureHandle texture, const char* name) {
	if (bgfx::isValid(texture)) {
		if (name && name != uniformName) {
			initialize(name);
		}

		if (!uniform.isValid()) {
			OtLogFatal("internal error: sampler not initialized");
		}

		bgfx::setTexture(unit, uniform.getHandle(), texture, flags);

	} else {
		submit(unit, name);
	}
}

void OtSampler::submit(int unit, OtCubeMap& cubemap, const char* name) {
	if (name && name != uniformName) {
		initialize(name);
	}

	if (isValid()) {
		bgfx::setTexture(unit, uniform.getHandle(), cubemap.getTextureHandle(), flags);

	} else {
		OtLogFatal("internal error: sampler not initialized before submission");
	}
}
