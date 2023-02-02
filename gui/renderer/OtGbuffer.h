//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtBgfxHelpers.h"
#include "OtSampler.h"


//
//	OtGbuffer
//

class OtGbuffer {
public:
	// clear all resources
	void clear();

	// see if framebuffer is valid
	bool isValid() { return gbuffer.isValid(); }

	// update frame buffer
	void update(int width, int height);

	// get framebuffer dimensions
	int getWidth() { return width; }
	int getHeight() { return height; }

	// get textures
	bgfx::TextureHandle getAlbedoTexture() { return albedoTexture.getHandle(); }
	bgfx::TextureHandle getPositionTexture() { return positionTexture.getHandle(); }
	bgfx::TextureHandle getNormalTexture() { return normalTexture.getHandle(); }
	bgfx::TextureHandle getPbrTexture() { return pbrTexture.getHandle(); }
	bgfx::TextureHandle getDepthTexture() { return depthTexture.getHandle(); }

	// get texture indices
	int getAlbedoTextureIndex() { return albedoTexture.getIndex(); }
	int getPositionTextureIndex() { return positionTexture.getIndex(); }
	int getNormalTextureIndex() { return normalTexture.getIndex(); }
	int getPbrTextureIndex() { return pbrTexture.getIndex(); }
	int getDepthTextureIndex() { return depthTexture.getIndex(); }

	// bind textures
	void bindAlbedoTexture(OtSampler& sampler, int unit);
	void bindSpecularTexture(OtSampler& sampler, int unit);
	void bindPositionTexture(OtSampler& sampler, int unit);
	void bindNormalTexture(OtSampler& sampler, int unit);
	void bindDepthTexture(OtSampler& sampler, int unit);

	// activate framebuffer in GPU for specified view
	void submit(bgfx::ViewId view);

private:
	// dimensions of framebuffer;
	int width = -1;
	int height = -1;

	// resource handles
	OtBgfxHandle<bgfx::TextureHandle> albedoTexture;
	OtBgfxHandle<bgfx::TextureHandle> positionTexture;
	OtBgfxHandle<bgfx::TextureHandle> normalTexture;
	OtBgfxHandle<bgfx::TextureHandle> pbrTexture;
	OtBgfxHandle<bgfx::TextureHandle> depthTexture;
	OtBgfxHandle<bgfx::FrameBufferHandle> gbuffer;
};
