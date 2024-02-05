//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>

#include "OtLibuv.h"

#include "OtBgfxHandle.h"
#include "OtSampler.h"
#include "OtShaderProgram.h"


//
//	OtCubeMap
//

class OtCubeMap {
public:
	// load cubemap from the specified file
	void load(const std::string& path);

	// clear the resources
	inline void clear() { cubemap.clear(); }

	// see if cubemap is valid
	inline bool isValid() { return cubemap.isValid(); }

	// return cubemap handle
	inline bgfx::TextureHandle getHandle() { return cubemap.getHandle(); }

	// return cubemap index
	inline uint16_t getIndex() {
		return isValid() ? cubemap.getIndex() : bgfx::kInvalidHandle;
	}

private:
	// cubemap texture
	OtBgfxHandle<bgfx::TextureHandle> cubemap;

	// temporary handles (in case we dynamically create a cubemap)
	uv_async_t* hdrConversionHandle = nullptr;
	OtBgfxHandle<bgfx::TextureHandle> tmpTexture;
	OtBgfxHandle<bgfx::FrameBufferHandle> framebuffers[6];
	OtSampler sampler{"s_equirectangularMap"};
	OtShaderProgram reprojectShader{"OtHdrReprojectVS", "OtHdrReprojectFS"};

	// specific cubemap loaders
	void loadJSON(const std::string& path);
	void loadCubemapImage(const std::string& path);
	void loadHdrImage(const std::string& path);

	// render the cubemap from an HDR image
	void renderCubemap();
};
