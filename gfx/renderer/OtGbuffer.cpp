//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtException.h"

#include "OtGbuffer.h"


//
//	OtGbuffer::clear
//

void OtGbuffer::clear() {
	albedoTexture.clear();
	normalTexture.clear();
	pbrTexture.clear();
	emissiveTexture.clear();
	depthTexture.clear();
	gbuffer.clear();
}


//
//	OtGbuffer::update
//

void OtGbuffer::update(int w, int h) {
	// update framebuffer if required
	if (!gbuffer.isValid() || w != width || h != height) {
		// clear old resources
		clear();

		// get texture flags
		uint64_t flags =
			BGFX_TEXTURE_RT |
			BGFX_SAMPLER_MIN_POINT |
			BGFX_SAMPLER_MAG_POINT |
			BGFX_SAMPLER_MIP_POINT |
			BGFX_SAMPLER_U_CLAMP |
			BGFX_SAMPLER_V_CLAMP;

		// create new textures
		albedoTexture = bgfx::createTexture2D(w, h, false, 1, bgfx::TextureFormat::RGBA8, flags);
		normalTexture = bgfx::createTexture2D(w, h, false, 1, bgfx::TextureFormat::RGBA16F, flags);
		pbrTexture = bgfx::createTexture2D(w, h, false, 1, bgfx::TextureFormat::RGBA16F, flags);
		emissiveTexture = bgfx::createTexture2D(w, h, false, 1, bgfx::TextureFormat::RGBA8, flags);
		depthTexture = bgfx::createTexture2D(w, h, false, 1, bgfx::TextureFormat::D32F, flags);

		// create gbuffer
		bgfx::TextureHandle textures[] = {
			albedoTexture.getHandle(),
			normalTexture.getHandle(),
			pbrTexture.getHandle(),
			emissiveTexture.getHandle(),
			depthTexture.getHandle()
		};

		gbuffer = bgfx::createFrameBuffer(sizeof(textures) / sizeof(textures[0]), textures);

		// remember dimensions
		width = w;
		height = h;
	}
}


//
//	OtGbuffer::bindAlbedoTexture
//

void OtGbuffer::bindAlbedoTexture(OtSampler& sampler, int unit) {
	sampler.submit(unit, albedoTexture.getHandle());
}


//
//	OtGbuffer::bindNormalTexture
//

void OtGbuffer::bindNormalTexture(OtSampler& sampler, int unit) {
	sampler.submit(unit, normalTexture.getHandle());
}


//
//	OtGbuffer::bindPbrTexture
//

void OtGbuffer::bindPbrTexture(OtSampler& sampler, int unit) {
	sampler.submit(unit, pbrTexture.getHandle());
}


//
//	OtGbuffer::bindEmissiveTexture
//

void OtGbuffer::bindEmissiveTexture(OtSampler& sampler, int unit) {
	sampler.submit(unit, emissiveTexture.getHandle());
}


//
//	OtGbuffer::bindDepthTexture
//

void OtGbuffer::bindDepthTexture(OtSampler& sampler, int unit) {
	sampler.submit(unit, depthTexture.getHandle());
}


//
//	OtGbuffer::submit
//

void OtGbuffer::submit(bgfx::ViewId view) {
	// attach gbuffer to view
	if (isValid()) {
		bgfx::setViewFrameBuffer(view, gbuffer.getHandle());

	} else {
		OtError("Internal error: Gbuffer not initialized before submission");
	}
}
