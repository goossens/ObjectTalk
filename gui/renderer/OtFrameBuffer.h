//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtBgfxHandle.h"

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
		rgbaFloat16Texture = bgfx::TextureFormat::RGBA16F,
		rgbaFloat32Texture = bgfx::TextureFormat::RGBA32F,
		d16Texture = bgfx::TextureFormat::D16,
		dFloatTexture = bgfx::TextureFormat::D32F
	};

	// constructors
	OtFrameBuffer() = default;
	OtFrameBuffer(int colorTextureType, int depthTextureType=noTexture, int antiAliasing=1, bool blitTarget=false);

	// initialize framebuffer
	void initialize(int colorTextureType, int depthTextureType=noTexture, int antiAliasing=1, bool blitTarget=false);

	// clear all resources
	void clear();

	// see if framebuffer is valid
	bool isValid() { return framebuffer.isValid(); }

	// update frame buffer
	void update(int width, int height);

	// get framebuffer dimensions
	int getWidth() { return width; }
	int getHeight() { return height; }

	// get textures
	bgfx::TextureHandle getColorTexture() { return colorTexture.getHandle(); }
	bgfx::TextureHandle getDepthTexture() { return depthTexture.getHandle(); }

	// get texture indices
	int getColorTextureIndex() { return colorTexture.getIndex(); }
	int getDepthTextureIndex() { return depthTexture.getIndex(); }

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
	bool blitTarget=false;

	// dimensions of framebuffer;
	int width = -1;
	int height = -1;

	// resource handles
	OtBgfxHandle<bgfx::TextureHandle> colorTexture;
	OtBgfxHandle<bgfx::TextureHandle> depthTexture;
	OtBgfxHandle<bgfx::FrameBufferHandle> framebuffer;
};
