//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <filesystem>

#include "OtBgfxHandle.h"


//
//	OtTexture
//

class OtTexture {
public:
	static constexpr int invalidIndex = bgfx::kInvalidHandle;

	// constructor
	OtTexture() = default;
	OtTexture(OtBgfxHandle<bgfx::TextureHandle> t, size_t w, size_t h) : texture(t), width(w), height(h) {}
	OtTexture(const std::filesystem::path& path);

	// clear the resources
	inline void clear() { texture.clear(); }

	// see if texture is valid
	inline bool isValid() { return texture.isValid(); }

	// load from file
	void loadFromFile(const std::filesystem::path& path);

	// load from memory (pixels must be RGBA in row order)
	void loadFromMemory(int width, int height, uint8_t* pixels);
	void loadFromMemory(int width, int height, float* pixels);

	// load from file in memory
	void loadFromFileInMemory(void* data, uint32_t size);

	// return texture handle
	bgfx::TextureHandle getTextureHandle();

	// return texture index
	inline uint16_t getTextureIndex() {
		return isValid() ? texture.getIndex() : bgfx::kInvalidHandle;
	}

	// get texture size
	inline size_t getWidth() { return width; }
	inline size_t getHeight() { return height; }

private:
	// texture
	OtBgfxHandle<bgfx::TextureHandle> texture;
	size_t width = 1;
	size_t height = 1;
};
