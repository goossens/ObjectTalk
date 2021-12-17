//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "gui.h"
#include "noisemap.h"


//
//	OtTexture
//

class OtTextureClass;
typedef std::shared_ptr<OtTextureClass> OtTexture;

class OtTextureClass : public OtGuiClass {
public:
	// constructor/destructor
	OtTextureClass();
	~OtTextureClass();

	// initialize
	OtObject init(size_t count, OtObject* parameters);

	// specify new image file
	OtObject loadImage(const std::string& file);

	// specify a noisemap to visualize
	OtObject setNoiseMap(OtObject object, size_t width, size_t height);

	// provide new pixels
	void setPixels(void* pixels, size_t size, bimg::TextureFormat::Enum format, size_t width, size_t height);

	// get texture information
	size_t getWidth() { return width; }
	size_t getHeight() { return height; }
	bimg::TextureFormat::Enum getFormat() { return format; }
	bgfx::TextureHandle getTextureHandle() { return texture; }

	// submit shader data to BGFX
	void submit(int stage, bgfx::UniformHandle uniform);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtTexture create();

	// get an empty dummy texture
	static OtTexture dummy();

private:
	// handle noisemap
	OtNoiseMap noisemap;
	size_t noisemapID;
	void processNoiseMap();

	// texture properties
	size_t width = 0;
	size_t height = 0;
	bimg::TextureFormat::Enum format;

	bimg::ImageContainer* image = nullptr;
	bgfx::TextureHandle texture = BGFX_INVALID_HANDLE;

	// static dummy texture (so we don't have to potentially create a bunch)
	static OtTexture dummyTexture;
};
