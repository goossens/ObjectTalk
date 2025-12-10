//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <cstdint>

#include "imgui.h"
#include "SDL3/SDL.h"

#include "OtLog.h"

#include "OtRenderTarget.h"
#include "OtTexture.h"


//
//	OtFrameBuffer
//

class OtFrameBuffer : public OtRenderTarget {
public:
	// constructors/destructor
	OtFrameBuffer();
	OtFrameBuffer(OtTexture::Format colorTextureType, OtTexture::Format depthTextureType=OtTexture::Format::none);
	~OtFrameBuffer();

	// initialize framebuffer
	void initialize(OtTexture::Format c, OtTexture::Format d=OtTexture::Format::none);

	// clear all resources
	void clear();

	// see if framebuffer is valid
	inline bool isValid() { return colorTextureType != OtTexture::Format::none || depthTextureType != OtTexture::Format::none; }

	// update frame buffer
	bool update(int w, int h);

	// get framebuffer dimensions
	inline int getWidth() { return width; }
	inline int getHeight() { return height; }

	// see if textures are available
	inline bool hasColorTexture() { return colorTextureType != OtTexture::Format::none; }
	inline bool hasDepthTexture() { return depthTexture.hasDepth(); }
	inline bool hasStencilTexture() { return depthTexture.hasStencil(); }

	// get texture types
	inline OtTexture::Format getColorTextureType() { return colorTextureType; }
	inline OtTexture::Format getDepthTextureType() { return depthTextureType; }

	// get textures
	inline OtTexture& getColorTexture() { return colorTexture; }
	inline OtTexture& getDepthTexture() { return depthTexture; }

	// get texture IDs (for Dear ImGUI use)
	inline ImTextureID getColorTextureID() { return colorTexture.getTextureID(); }
	inline ImTextureID getDepthTextureID() { return depthTexture.getTextureID(); }

private:
	// properties
	OtTexture::Format colorTextureType = OtTexture::Format::none;
	OtTexture::Format depthTextureType = OtTexture::Format::none;

	// dimensions of framebuffer
	int width = -1;
	int height = -1;

	// resources
	OtTexture colorTexture;
	OtTexture depthTexture;

	// render target description
	SDL_GPUColorTargetInfo colorTargetInfo;
	SDL_GPUDepthStencilTargetInfo depthStencilTargetInfo;
	OtRenderTargetInfo info;

	// get render target information
	friend class OtRenderPass;

	OtRenderTargetInfo* getRenderTargetInfo(
		bool clearColorTexture,
		bool clearDepthTexture,
		bool clearStencilTexture,
		glm::vec4 clearColorValue,
		float clearDepthValue,
		std::uint8_t clearStencilValue) override;
};
