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

#include "OtBgfxHandle.h"


//
//	OtTexture
//

class OtTexture {
public:
	static constexpr int invalidIndex = bgfx::kInvalidHandle;

	// constructor
	OtTexture() = default;
	OtTexture(OtBgfxHandle<bgfx::TextureHandle> t, int w, int h) : texture(t), width(w), height(h) {}
	OtTexture(const std::string& path);

	// clear the resources
	void clear();

	// see if texture is valid
	inline bool isValid() { return texture.isValid(); }

	// load from file
	void loadFromFile(const std::string& path);

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
	inline int getWidth() { return width; }
	inline int getHeight() { return height; }

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
	int version = 0;
};
