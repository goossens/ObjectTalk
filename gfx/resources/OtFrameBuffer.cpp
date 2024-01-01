//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtLog.h"

#include "OtFrameBuffer.h"


//
//	OtFrameBuffer:OtFrameBuffer
//

OtFrameBuffer::OtFrameBuffer(int colorTextureType, int depthTextureType, int antiAliasing, bool blitTarget) {
	initialize(colorTextureType, depthTextureType, antiAliasing, blitTarget);
}


//
//	OtFrameBuffer::initialize
//

void OtFrameBuffer::initialize(int c, int d, int a, bool b) {
	if (colorTextureType != c || depthTextureType != d || antiAliasing != a || colorTextureType != c) {
		clear();
		colorTextureType = c;
		depthTextureType = d;
		antiAliasing = a;
		blitTarget = b;
	}
}


//
//	OtFrameBuffer::clear
//

void OtFrameBuffer::clear() {
	// release resources (if required)
	colorTexture.clear();
	depthTexture.clear();
	framebuffer.clear();

	// clear other fields
	width = -1;
	height = -1;
}


//
//	computeTextureRtMsaaFlag
//

static inline uint64_t computeTextureRtMsaaFlag(int aa) {
	switch (aa) {
		case 2:
			return BGFX_TEXTURE_RT_MSAA_X2;

		case 4:
			return BGFX_TEXTURE_RT_MSAA_X4;

		case 8:
			return BGFX_TEXTURE_RT_MSAA_X8;

		case 16:
			return BGFX_TEXTURE_RT_MSAA_X16;

		default:
			return BGFX_TEXTURE_RT;
	}
}


//
//	OtFrameBuffer::update
//

void OtFrameBuffer::update(int w, int h) {
	// update framebuffer if required
	if (!framebuffer.isValid() || w != width || h != height) {
		// clear old resources
		clear();

		// create new textures
		uint64_t flags = computeTextureRtMsaaFlag(antiAliasing);
		uint64_t blit = blitTarget ? BGFX_TEXTURE_BLIT_DST : 0;

		if (colorTextureType != noTexture) {
			colorTexture = bgfx::createTexture2D(w, h, false, 1, (bgfx::TextureFormat::Enum) colorTextureType, flags | blit);
		}

		if (depthTextureType != noTexture) {
			depthTexture = bgfx::createTexture2D(w, h, false, 1, (bgfx::TextureFormat::Enum) depthTextureType, flags | blit);
		}

		// create framebuffer
		bgfx::TextureHandle textures[2] = {colorTexture.getHandle(), depthTexture.getHandle()};

		if (colorTextureType && depthTextureType) {
			framebuffer = bgfx::createFrameBuffer(2, textures);

		} else if (colorTextureType) {
			framebuffer = bgfx::createFrameBuffer(1, &textures[0]);

		} else if (depthTextureType) {
			framebuffer = bgfx::createFrameBuffer(1, &textures[1]);

		} else {
			OtLogFatal("Internal error: You can't have a FrameBuffer without Textures");
		}

		// remember dimensions
		width = w;
		height = h;
	}
}


//
//	OtFrameBuffer::bindColorTexture
//

void OtFrameBuffer::bindColorTexture(OtSampler& sampler, int unit) {
	sampler.submit(unit, colorTexture.getHandle());
}


//
//	OtFrameBuffer::bindDepthTexture
//

void OtFrameBuffer::bindDepthTexture(OtSampler& sampler, int unit) {
	sampler.submit(unit, depthTexture.getHandle());
}


//
//	OtFrameBuffer::submit
//

void OtFrameBuffer::submit(bgfx::ViewId view) {
	// attach framebuffer to view
	if (isValid()) {
		bgfx::setViewFrameBuffer(view, framebuffer.getHandle());

	} else {
		OtLogFatal("Internal error: IndexBuffer not initialized before submission");
	}
}
