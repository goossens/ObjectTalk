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
#include <cmath>
#include <string>

#include "OtLibuv.h"

#include "OtBgfxHandle.h"
#include "OtSampler.h"
#include "OtShaderProgram.h"
#include "OtUniformVec4.h"


//
//	OtCubeMap
//

class OtCubeMap {
public:
	// create an empty cubemap
	void create(int size, bool mip, int layers, int format, uint64_t flags);

	// load cubemap from the specified file
	void load(const std::string& path);

	// clear the resources
	inline void clear() { cubemap.clear(); }

	// see if cubemap is valid
	inline bool isValid() { return cubemap.isValid(); }

	// return cubemap handle
	bgfx::TextureHandle getHandle();

	// return cubemap index
	inline uint16_t getIndex() {
		return isValid() ? cubemap.getIndex() : bgfx::kInvalidHandle;
	}

	// get cubemap information
	inline int getSize() { return size; }
	inline bool hasMip() { return mip; }
	inline int getMipLevels() { return mip ? (int) std::ceil(std::log2(float(size))) : 0; }
	inline int getLayers() { return layers; }
	inline int getFormat() { return format; }

	// version management
	inline void setVersion(int v) { version = v; }
	inline int getVersion() { return version; }
	inline void incrementVersion() { version++; }

	// see if cubemaps are identical
	inline bool operator==(OtCubeMap& rhs) {
		return cubemap == rhs.cubemap && size == rhs.size && version == rhs.version;
	}

	inline bool operator!=(OtCubeMap& rhs) {
		return !operator==(rhs);
	}

private:
	// cubemap texture
	OtBgfxHandle<bgfx::TextureHandle> cubemap;
	int size = 1;
	bool mip = false;
	int layers = 1;
	int format;
	int version = 0;

	// temporary handles (in case we dynamically create a cubemap)
	uv_async_t* hdrConversionHandle = nullptr;
	OtBgfxHandle<bgfx::TextureHandle> tmpTexture;
	OtBgfxHandle<bgfx::FrameBufferHandle> framebuffers[6];
	OtSampler remapSampler{"s_equirectangularMap"};
	OtShaderProgram reprojectShader{"OtHdrReprojectVS", "OtHdrReprojectFS"};

	// specific cubemap loaders
	void loadJSON(const std::string& path);
	void loadCubemapImage(const std::string& path);
	void loadHdrImage(const std::string& path);

	// render the cubemap from an HDR image
	void renderCubemap();
};
