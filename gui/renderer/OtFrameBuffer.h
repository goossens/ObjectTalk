//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "bgfx/bgfx.h"

#include "OtSampler.h"


//
//	OtFrameBuffer
//

class OtFrameBuffer {
public:
	// texture types
	enum {
		noTexture = 0,
		rgba8Texture = bgfx::TextureFormat::RGBA8,
		rgba16Texture = bgfx::TextureFormat::RGBA16,
		rgbaFloatTexture = bgfx::TextureFormat::RGBA32F,
		d16Texture = bgfx::TextureFormat::D16,
		dFloatTexture = bgfx::TextureFormat::D32F
	};

	// constructors/destructor
	OtFrameBuffer() = default;
	OtFrameBuffer(int colorTextureType, int depthTextureType=noTexture, int antiAliasing=1);

	~OtFrameBuffer();

	// initialize framebuffer
	void initialize(int colorTextureType, int depthTextureType=noTexture, int antiAliasing=1);

	// clear all resources
	void clear();

	// update frame buffer
	void update(int width, int height);

	// get framebuffer dimensions
	int getWidth() { return width; }
	int getHeight() { return height; }

	// get textures
	bgfx::TextureHandle getColorTexture() { return colorTexture; }
	bgfx::TextureHandle getDepthTexture() { return depthTexture; }

	// get texture indices
	int getColorTextureIndex() { return colorTexture.idx; }
	int getDepthTextureIndex() { return depthTexture.idx; }

	// bind textures
	void bindColorTexture(OtSampler& sampler, int unit);
	void bindDepthTexture(OtSampler& sampler, int unit);

	// activate framebuffer in GPU for specified view
	void submit(bgfx::ViewId view);

private:
	// properties
	int colorTextureType = noTexture;
	int depthTextureType = noTexture;
	int antiAliasing = 1;

	// dimensions of framebuffer;
	int width = -1;
	int height = -1;

	// resource handles
	bgfx::TextureHandle colorTexture = BGFX_INVALID_HANDLE;
	bgfx::TextureHandle depthTexture = BGFX_INVALID_HANDLE;
	bgfx::FrameBufferHandle framebuffer = BGFX_INVALID_HANDLE;
};
