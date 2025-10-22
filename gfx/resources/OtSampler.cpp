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
// OtSampler::clear
//

void OtSampler::clear() {
	name.clear();
	flags = defaultSampling;
	uniform.clear();
}


//
//	OtSampler::setFlags
//

void OtSampler::setFlags(uint64_t f) {
	if (flags != f) {
		flags = f;
		uniform.clear();
	}
}


//
//	OtSampler::submit
//

void OtSampler::submit(int unit, OtTexture& texture) {
	// see if texture is valid/ready
	if (texture.isValid()) {
		// generate resource (if required)
		if (!isValid()) {
			createUniform();
		}

		// submit texture
		bgfx::setTexture(static_cast<uint8_t>(unit), uniform.getHandle(), texture.getHandle(), static_cast<uint32_t>(flags));

	} else {
		// submit dummy texture
		submitDummyTexture(unit);
	}
}

void OtSampler::submit(int unit, bgfx::TextureHandle texture) {
	// see if texture is valid/ready
	if (bgfx::isValid(texture)) {
		// generate resource (if required)
		if (!isValid()) {
			createUniform();
		}

		// submit texture
		bgfx::setTexture(static_cast<uint8_t>(unit), uniform.getHandle(), texture, static_cast<uint32_t>(flags));

	} else {
		submitDummyTexture(unit);
	}
}

void OtSampler::submit(int unit, OtCubeMap& cubemap) {
	// see if cubemap is valid/ready
	if (cubemap.isValid()) {
		// generate resource (if required)
		if (!isValid()) {
			createUniform();
		}

		// submit texture
		bgfx::setTexture(static_cast<uint8_t>(unit), uniform.getHandle(), cubemap.getHandle(), static_cast<uint32_t>(flags));

	} else {
		submitDummyCubeMap(unit);
	}
}


//
//	OtSampler::submitDummyTexture
//

void OtSampler::submitDummyTexture(int unit) {
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

void OtSampler::submitDummyCubeMap(int unit) {
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
	uniform = bgfx::createUniform(name.c_str(), bgfx::UniformType::Sampler);
}
