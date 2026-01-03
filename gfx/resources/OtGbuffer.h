//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "imgui.h"
#include "SDL3/SDL.h"

#include "OtRenderTarget.h"
#include "OtTexture.h"


//
//	OtGbuffer
//

class OtGbuffer : public OtRenderTarget {
public:
	// constructor/destructor
	OtGbuffer();
	~OtGbuffer();

	// clear all resources
	void clear();

	// see if gbuffer is valid
	inline bool isValid() { return valid; }

	// update gbuffer
	bool update(int w, int h);

	// get framebuffer dimensions
	inline int getWidth() { return width; }
	inline int getHeight() { return height; }

		// get textures
	inline OtTexture& getAlbedoTexture() { return albedoTexture; }
	inline OtTexture& getNormalTexture() { return normalTexture; }
	inline OtTexture& getPbrTexture() { return pbrTexture; }
	inline OtTexture& getEmissiveTexture() { return emissiveTexture; }
	inline OtTexture& getDepthTexture() { return depthTexture; }

	// get texture IDs (for Dear ImGUI use)
	inline ImTextureID getAlbedoTextureID() { return albedoTexture.getTextureID(); }
	inline ImTextureID getNormalTextureID() { return normalTexture.getTextureID(); }
	inline ImTextureID getPbrTextureID() { return pbrTexture.getTextureID(); }
	inline ImTextureID getEmissiveTextureID() { return emissiveTexture.getTextureID(); }
	inline ImTextureID getDepthTextureID() { return depthTexture.getTextureID(); }

private:
	// dimensions of gbuffer
	int width = -1;
	int height = -1;

	// state
	bool valid = false;

	// resources
	OtTexture albedoTexture;
	OtTexture normalTexture;
	OtTexture pbrTexture;
	OtTexture emissiveTexture;
	OtTexture depthTexture;

	// render target description
	SDL_GPUColorTargetInfo colorTargetInfo[4];
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
