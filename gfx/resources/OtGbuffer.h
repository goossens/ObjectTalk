//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
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
//	OtGbuffer
//

class OtGbuffer {
public:
	// clear all resources
	void clear();

	// see if gbuffer is valid
	inline bool isValid() { return gbuffer.isValid(); }

	// update gbuffer
	void update(int width, int height);

	// get framebuffer dimensions
	inline int getWidth() { return width; }
	inline int getHeight() { return height; }

	// get texture handles
	inline bgfx::TextureHandle getAlbedoTextureHandle() { return albedoTexture.getHandle(); }
	inline bgfx::TextureHandle getNormalTextureHandle() { return normalTexture.getHandle(); }
	inline bgfx::TextureHandle getPbrTextureHandle() { return pbrTexture.getHandle(); }
	inline bgfx::TextureHandle getEmissiveTextureHandle() { return emissiveTexture.getHandle(); }
	inline bgfx::TextureHandle getDepthTextureHandle() { return depthTexture.getHandle(); }

	// get texture indices
	inline int getAlbedoTextureIndex() { return albedoTexture.getIndex(); }
	inline int getNormalTextureIndex() { return normalTexture.getIndex(); }
	inline int getPbrTextureIndex() { return pbrTexture.getIndex(); }
	inline int getEmissiveTextureIndex() { return emissiveTexture.getIndex(); }
	inline int getDepthTextureIndex() { return depthTexture.getIndex(); }

	// bind textures
	void bindAlbedoTexture(OtSampler& sampler, int unit);
	void bindNormalTexture(OtSampler& sampler, int unit);
	void bindPbrTexture(OtSampler& sampler, int unit);
	void bindEmissiveTexture(OtSampler& sampler, int unit);
	void bindDepthTexture(OtSampler& sampler, int unit);

	// activate gbuffer in GPU for specified view
	void submit(bgfx::ViewId view);

private:
	// dimensions of framebuffer;
	int width = -1;
	int height = -1;

	// resource handles
	OtBgfxHandle<bgfx::TextureHandle> albedoTexture;
	OtBgfxHandle<bgfx::TextureHandle> normalTexture;
	OtBgfxHandle<bgfx::TextureHandle> pbrTexture;
	OtBgfxHandle<bgfx::TextureHandle> emissiveTexture;
	OtBgfxHandle<bgfx::TextureHandle> depthTexture;
	OtBgfxHandle<bgfx::FrameBufferHandle> gbuffer;
};
