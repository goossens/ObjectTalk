//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "imgui.h"

#include "OtBgfxHandle.h"

#include "OtSampler.h"
#include "OtTexture.h"


//
//	OtFrameBuffer
//

class OtFrameBuffer {
public:
	// constructors
	OtFrameBuffer() = default;
	OtFrameBuffer(int colorTextureType, int depthTextureType=OtTexture::noTexture, int antiAliasing=1, bool blitTarget=false);

	// initialize framebuffer
	void initialize(int colorTextureType, int depthTextureType=OtTexture::noTexture, int antiAliasing=1, bool blitTarget=false);

	// clear all resources
	void clear();

	// see if framebuffer is valid
	inline bool isValid() { return framebuffer.isValid(); }

	// update frame buffer
	void update(int width, int height);

	// get framebuffer dimensions
	inline int getWidth() { return width; }
	inline int getHeight() { return height; }

	// see if textures are available
	inline bool hasColorTexture() { return colorTextureType != OtTexture::noTexture; }
	inline bool hasDepthTexture() { return OtTexture::hasDepth(depthTextureType); }
	inline bool hasStencilTexture() { return OtTexture::hasStencil(depthTextureType); }

	// get texture types
	inline int getColorTextureType() { return colorTextureType; }
	inline int getDepthTextureType() { return depthTextureType; }

	// get textures
	inline OtTexture getColorTexture() { return OtTexture(colorTexture, width, height, colorTextureType); }
	inline OtTexture getDepthTexture() { return OtTexture(depthTexture, width, height, depthTextureType); }

	// get texture handles
	inline bgfx::TextureHandle getColorTextureHandle() { return colorTexture.getHandle(); }
	inline bgfx::TextureHandle getDepthTextureHandle() { return depthTexture.getHandle(); }

	// get texture indices
	inline uint16_t getColorTextureIndex() { return colorTexture.getIndex(); }
	inline uint16_t getDepthTextureIndex() { return depthTexture.getIndex(); }

	// get texture IDs (for Dear ImGUI use)
	inline ImTextureID getColorTextureID() { return colorTexture.isValid() ? static_cast<ImTextureID>(colorTexture.getIndex()) : ImTextureID_Invalid; }
	inline ImTextureID getDepthTextureID() { return depthTexture.isValid() ? static_cast<ImTextureID>(depthTexture.getIndex()) : ImTextureID_Invalid; }

	// bind textures
	void bindColorTexture(OtSampler& sampler, int unit);
	void bindDepthTexture(OtSampler& sampler, int unit);

	// activate framebuffer in GPU for specified view
	void submit(bgfx::ViewId view);

private:
	// properties
	int colorTextureType = OtTexture::noTexture;
	int depthTextureType = OtTexture::noTexture;
	int antiAliasing = 1;
	bool blitTarget = false;

	// dimensions of framebuffer;
	int width = -1;
	int height = -1;

	// resource handles
	OtBgfxHandle<bgfx::TextureHandle> colorTexture;
	OtBgfxHandle<bgfx::TextureHandle> depthTexture;
	OtBgfxHandle<bgfx::FrameBufferHandle> framebuffer;
};
