//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>

#include "OtBgfxHelpers.h"


//
//	OtTexture
//

class OtTexture {
public:
	// constructor
	OtTexture() = default;
	OtTexture(const std::string& filename, bool mipmap=false);

	// clear the resources
	void clear() { texture.clear(); }

	// see if texture is valid
	bool isValid() { return texture.isValid(); }

	// load from memory (pixels must be RGBA in row order)
	void loadFromMemory(int width, int height, uint8_t* pixels);

	// load from file
	void loadFromFile(const std::string& filename, bool mipmap=false);

	// load from file in memory
	void loadFromFileInMemory(void* data, uint32_t size);

	// return texture handle
	bgfx::TextureHandle getTextureHandle();

	// return texture index
	int getTextureIndex() { return texture.getIndex(); }

	// get texture size
	size_t getWidth() { return width; }
	size_t getHeight() { return height; }

private:
	// texture
	OtBgfxHandle<bgfx::TextureHandle> texture;
	size_t width = 1;
	size_t height = 1;
};
