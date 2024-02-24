//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <cstdint>
#include <string>

#include "OtBgfxHandle.h"
#include "OtImage.h"


//
//	OtTexture
//

class OtTexture {
public:
	// invalid handle
	static constexpr int invalidIndex = bgfx::kInvalidHandle;

	// texture types
	static constexpr int noTexture = 0;
	static constexpr int r8Texture = bgfx::TextureFormat::R8;
	static constexpr int r16Texture = bgfx::TextureFormat::R16;
	static constexpr int rFloat32Texture = bgfx::TextureFormat::R32F;
	static constexpr int rgFloat16Texture = bgfx::TextureFormat::RG16F;
	static constexpr int rgFloat32Texture = bgfx::TextureFormat::RG32F;
	static constexpr int rgba8Texture = bgfx::TextureFormat::RGBA8;
	static constexpr int rgba16Texture = bgfx::TextureFormat::RGBA16;
	static constexpr int rgbaFloat16Texture = bgfx::TextureFormat::RGBA16F;
	static constexpr int rgbaFloat32Texture = bgfx::TextureFormat::RGBA32F;
	static constexpr int d16Texture = bgfx::TextureFormat::D16;
	static constexpr int dFloatTexture = bgfx::TextureFormat::D32F;

	// texture flags
	static constexpr uint64_t defaultSampling = UINT32_MAX;
	static constexpr uint64_t linearSampling = BGFX_SAMPLER_NONE;
	static constexpr uint64_t pointSampling = BGFX_SAMPLER_POINT;
	static constexpr uint64_t anisotropicSampling = BGFX_SAMPLER_MIN_ANISOTROPIC | BGFX_SAMPLER_MAG_ANISOTROPIC;
	static constexpr uint64_t repeatSampling = BGFX_SAMPLER_NONE;
	static constexpr uint64_t clampSampling = BGFX_SAMPLER_UVW_CLAMP;
	static constexpr uint64_t mirrorSampling = BGFX_SAMPLER_UVW_MIRROR;

	static constexpr uint64_t computeWrite = BGFX_TEXTURE_COMPUTE_WRITE;
	static constexpr uint64_t blitDst = BGFX_TEXTURE_BLIT_DST;
	static constexpr uint64_t readBack = BGFX_TEXTURE_READ_BACK;

	// constructors
	OtTexture() = default;
	OtTexture(OtBgfxHandle<bgfx::TextureHandle> t, int w, int h, int f) : texture(t), width(w), height(h), format(f) {}
	OtTexture(const std::string& path);

	// clear the resources
	void clear();

	// see if texture is valid
	inline bool isValid() { return texture.isValid(); }

	// create an empty texture
	void create(int w, int h, int format, uint64_t flags);

	// load from image
	void loadFromImage(OtImage& image);

	// load from file
	void loadFromFile(const std::string& path);

	// load from memory (pixels must be RGBA in row order)
	void loadFromMemory(int width, int height, uint8_t* pixels);
	void loadFromMemory(int width, int height, float* pixels);

	// load from file in memory
	void loadFromFileInMemory(void* data, uint32_t size);

	// return texture handle
	bgfx::TextureHandle getHandle();

	// return texture index
	inline uint16_t getIndex() { return isValid() ? texture.getIndex() : bgfx::kInvalidHandle; }

	// get texture properties
	inline int getWidth() { return width; }
	inline int getHeight() { return height; }
	inline int getFormat() { return format; }

	// version management
	inline void setVersion(int v) { version = v; }
	inline int getVersion() { return version; }
	inline void incrementVersion() { version++; }

	// see if textures are identical
	inline bool operator==(OtTexture& rhs) {
		return texture == rhs.texture && width == rhs.width && height == rhs.height && version == rhs.version;
	}

	inline bool operator!=(OtTexture& rhs) {
		return !operator==(rhs);
	}

private:
	// texture
	OtBgfxHandle<bgfx::TextureHandle> texture;
	int width = 1;
	int height = 1;
	int format = noTexture;
	int version = 0;
};
