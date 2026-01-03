//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <cmath>
#include <memory>
#include <string>

#include "SDL3/SDL.h"

#include "OtLibuv.h"

#include "OtImage.h"
#include "OtGpu.h"
#include "OtRenderPipeline.h"
#include "OtSampler.h"


//
//	OtCubeMap
//

class OtCubeMap {
public:
	// image formats
	enum class Format {
		none = SDL_GPU_TEXTUREFORMAT_INVALID,
		rgba8 = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
		rgba16 = SDL_GPU_TEXTUREFORMAT_R16G16B16A16_FLOAT,
		rgba32 = SDL_GPU_TEXTUREFORMAT_R32G32B32A32_FLOAT
	};

	// create an empty cubemap
	void create(Format format, int size, bool mip=false);

	// load cubemap from the specified file
	void load(const std::string& path, bool async=false);

	// clear the resources
	void clear();

	// see if cubemap is valid
	inline bool isValid() { return cubemap != nullptr; }

	// get cubemap information
	inline int getSize() { return size; }
	inline bool hasMip() { return mip; }
	inline int getMipLevels() { return mip ? (int) std::ceil(std::log2(float(size))) : 0; }
	inline Format getFormat() { return format; }

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
	std::shared_ptr<SDL_GPUTexture> cubemap;

	// memory manage SDL resource
	void assign(SDL_GPUTexture* newTexture);

	// properties
	Format format = Format::none;
	int size = 1;
	bool mip = false;
	int version = 0;

	// variables for async loading from files
	uv_async_t* asyncHandle = nullptr;
	std::unique_ptr<std::byte[]> imageData;
	std::unique_ptr<OtImage> asyncImage;
	size_t bytesPerImage;

	OtRenderPipeline hdrPipeline;
	OtSampler hdrSampler{OtSampler::Filter::linear, OtSampler::Addressing::clamp};

	// specific cubemap loaders
	void loadJSON(const std::string& path, bool async);
	void loadHdrImage(const std::string& path, bool async);

	// local support functions
	void createCubemapFromSides();
	void createCubemapFromHDR();

	// get the raw texture object
	friend class OtComputePass;
	friend class OtRenderPass;

	inline SDL_GPUTexture* getTexture() { return isValid() ? cubemap.get() : OtGpu::instance().dummyCubeMap; }
};
