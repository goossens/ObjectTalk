//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtException.h"

#include "OtFrameBuffer.h"


//
//	OtFrameBuffer:OtFrameBuffer
//

OtFrameBuffer::OtFrameBuffer(int colorTextureType, int depthTextureType, int antiAliasing) {
	initialize(colorTextureType, depthTextureType, antiAliasing);
}


//
//	OtFrameBuffer:~OtFrameBuffer
//

OtFrameBuffer::~OtFrameBuffer() {
	clear();
}


//
//	OtFrameBuffer::initialize
//

void OtFrameBuffer::initialize(int c, int d, int a) {
	clear();
	colorTextureType = c;
	depthTextureType = d;
	antiAliasing = a;
}


//
//	OtFrameBuffer::clear
//

void OtFrameBuffer::clear() {
	// release resources (if required)
	if (bgfx::isValid(framebuffer)) {
		bgfx::destroy(framebuffer);
		framebuffer = BGFX_INVALID_HANDLE;
	}

	if (bgfx::isValid(colorTexture)) {
		bgfx::destroy(colorTexture);
		colorTexture = BGFX_INVALID_HANDLE;
	}

	if (bgfx::isValid(depthTexture)) {
		bgfx::destroy(depthTexture);
		depthTexture = BGFX_INVALID_HANDLE;
	}

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
	if (!bgfx::isValid(framebuffer) || w != width || h != height) {
		// clear old resources
		clear();

		// create new textures
		auto flags = computeTextureRtMsaaFlag(antiAliasing) | BGFX_SAMPLER_COMPARE_LEQUAL | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP;

		if (colorTextureType != noTexture) {
			colorTexture = bgfx::createTexture2D(w, h, false, 1, (bgfx::TextureFormat::Enum) colorTextureType, flags);
		}

		if (depthTextureType != noTexture) {
			depthTexture = bgfx::createTexture2D(w, h, false, 1, (bgfx::TextureFormat::Enum) depthTextureType, flags);
		}

		// create framebuffer
		if (colorTextureType && depthTextureType) {
			bgfx::TextureHandle textures[2] = {colorTexture, depthTexture};
			framebuffer = bgfx::createFrameBuffer(2, textures);

		} else if (colorTextureType) {
			framebuffer = bgfx::createFrameBuffer(1, &colorTexture);

		} else if (depthTextureType) {
			framebuffer = bgfx::createFrameBuffer(1, &depthTexture);

		} else {
			OtExcept("Internal error: You can't have a FrameBuffer without Textures");
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
	sampler.submit(unit, colorTexture);
}


//
//	OtFrameBuffer::bindDepthTexture
//

void OtFrameBuffer::bindDepthTexture(OtSampler& sampler, int unit) {
	sampler.submit(unit, depthTexture);
}


//
//	OtFrameBuffer::submit
//

void OtFrameBuffer::submit(bgfx::ViewId view) {
	// attach framebuffer to view
	bgfx::setViewFrameBuffer(view, framebuffer);
}
