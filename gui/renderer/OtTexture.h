//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>

#include "bgfx/bgfx.h"


//
//	OtTexture
//

class OtTexture {
public:
	// constructor/destructor
	OtTexture() = default;
	OtTexture(const std::string& filename, bool mipmap=false);
	~OtTexture();

	// clear the resources
	void clear();

	// load from memory (pixels must be RGBA in row order)
	void loadFromMemory(int width, int height, uint8_t* pixels);

	// load from file
	void loadFromFile(const std::string& filename, bool mipmap=false);

	// see if texture is valid
	bool isValid() { return bgfx::isValid(texture); }

	// return texture handle
	bgfx::TextureHandle getTextureHandle();

	// return texture index
	int getTextureIndex() { return texture.idx; }

	// get texture size
	size_t getWidth() { return width; }
	size_t getHeight() { return height; }

private:
	// texture
	bgfx::TextureHandle texture = BGFX_INVALID_HANDLE;
	size_t width = 1;
	size_t height = 1;
};
