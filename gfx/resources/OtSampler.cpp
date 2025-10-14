//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
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

void OtSampler::initialize(const char* n, uint64_t f) {
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
	name.clear();
	uniform.clear();
}


//
//	OtSampler::submit
//

void OtSampler::submit(int unit, OtTexture& texture, const char* samplerName) {
	// see if texture is valid/ready
	if (texture.isValid()) {
		// initialize sampler if new name is provided
		if (samplerName) {
			initialize(samplerName, linearSampling | repeatSampling);
		}

		// generate resource (if required)
		if (!isValid()) {
			createUniform();
		}

		// submit texture
		bgfx::setTexture(static_cast<uint8_t>(unit), uniform.getHandle(), texture.getHandle(), static_cast<uint32_t>(flags));

	} else {
		// submit dummy texture
		submitDummyTexture(unit, samplerName);
	}
}

void OtSampler::submit(int unit, bgfx::TextureHandle texture, const char* samplerName) {
	// see if texture is valid/ready
	if (bgfx::isValid(texture)) {
		// initialize sampler if new name is provided
		if (samplerName) {
			initialize(samplerName, linearSampling | repeatSampling);
		}

		// generate resource (if required)
		if (!isValid()) {
			createUniform();
		}

		// submit texture
		bgfx::setTexture(static_cast<uint8_t>(unit), uniform.getHandle(), texture, static_cast<uint32_t>(flags));

	} else {
		submitDummyTexture(unit, samplerName);
	}
}

void OtSampler::submit(int unit, OtCubeMap& cubemap, const char* samplerName) {
	// see if cubemap is valid/ready
	if (cubemap.isValid()) {
		// initialize sampler if new name is provided
		if (samplerName) {
			initialize(samplerName, linearSampling | repeatSampling);
		}

		// generate resource (if required)
		if (!isValid()) {
			createUniform();
		}

		// submit texture
		bgfx::setTexture(static_cast<uint8_t>(unit), uniform.getHandle(), cubemap.getHandle(), static_cast<uint32_t>(flags));

	} else {
		submitDummyCubeMap(unit, samplerName);
	}
}


//
//	OtSampler::submitDummyTexture
//

void OtSampler::submitDummyTexture(int unit, const char* samplerName) {
	// initialize sampler if new name is provided
	if (samplerName) {
		initialize(samplerName, linearSampling | repeatSampling);
	}

	// generate resource (if required)
	if (!isValid()) {
		createUniform();
	}

	// submit dummy texture
	OtTexture dummy;
	bgfx::setTexture(static_cast<uint8_t>(unit), uniform.getHandle(), dummy.getHandle(), static_cast<uint32_t>(flags));
}


//
//	OtSampler::submitDummyCubeMap
//

void OtSampler::submitDummyCubeMap(int unit, const char* samplerName) {
	// initialize sampler if new name is provided
	if (samplerName) {
		initialize(samplerName, linearSampling | repeatSampling);
	}

	// generate resource (if required)
	if (!isValid()) {
		createUniform();
	}

	// submit dummy texture
	OtCubeMap dummy;
	bgfx::setTexture(static_cast<uint8_t>(unit), uniform.getHandle(), dummy.getHandle(), static_cast<uint32_t>(flags));
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
