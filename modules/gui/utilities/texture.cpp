//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "bx/file.h"

#include "ot/exception.h"
#include "ot/function.h"

#include "image.h"
#include "texture.h"


//
//	Globals
//

static bx::DefaultAllocator allocator;


//
//	OtTextureClass::OtTextureClass
//

OtTextureClass::OtTextureClass() {
	// create dummy texture
	width = 1;
	height = 1;
	format = bimg::TextureFormat::R8;

	image = bimg::imageAlloc(&allocator, format, width, height, 0, 1, false, false);
	texture = bgfx::createTexture2D(width, height, 0, 1, bgfx::TextureFormat::Enum(format));
}


//
//	OtTextureClass::~OtTextureClass
//

OtTextureClass::~OtTextureClass() {
	// release resources
	bimg::imageFree(image);
	bgfx::destroy(texture);
}


//
//	OtTextureClass::init
//

OtObject OtTextureClass::init(size_t count, OtObject* parameters) {
	switch (count) {
		case 1:
			loadImage(parameters[0]->operator std::string());

		case 0:
			break;

		default:
			OtExcept("[Texture] constructor expects up to 1 arguments (not %ld)", count);
	}

	return nullptr;
}


//
//	OtTextureClass::loadImage
//

OtObject OtTextureClass::loadImage(const std::string& file) {
	// clear previous image
	bimg::imageFree(image);

	// load named texture
	image = OtLoadImage(file);
	const bgfx::Memory* mem = bgfx::makeRef(image->m_data, image->m_size);

	// create new texture if required
	if (image->m_width != width || image->m_height != height || image->m_format != format) {
		// destroy old texture
		bgfx::destroy(texture);

		// create a new texture
		texture = bgfx::createTexture2D(
			uint16_t(image->m_width),
			uint16_t(image->m_height),
			1 < image->m_numMips,
			image->m_numLayers,
			bgfx::TextureFormat::Enum(image->m_format),
			BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE,
			mem);

		// update state
		width = image->m_width;
		height = image->m_height;
		format = image->m_format;

	} else {
		// update texture
		bgfx::updateTexture2D(texture, 1, 0, 0, 0, width, height, mem);
	}

	return shared();
}


//
//	OtTextureClass::setPixels
//

void OtTextureClass::setPixels(void* pixels, size_t size, bimg::TextureFormat::Enum f, size_t w, size_t h) {
	// create new image and texture if required
	if (image->m_size != size || image->m_width != w || image->m_height != h || image->m_format != f) {
		bimg::imageFree(image);
		bgfx::destroy(texture);

		width = w;
		height = h;
		format = f;

		image = bimg::imageAlloc(&allocator, format, width, height, 0, 1, false, false, pixels);
		const bgfx::Memory* mem = bgfx::makeRef(image->m_data, image->m_size);

		texture = bgfx::createTexture2D(width, height, false, 1, bgfx::TextureFormat::Enum(format));
		bgfx::updateTexture2D(texture, 0, 0, 0, 0, width, height, mem);

	} else {
		// update image and texture
		std::memcpy(image->m_data, pixels, size);
		const bgfx::Memory* mem = bgfx::makeRef(image->m_data, image->m_size);
		bgfx::updateTexture2D(texture, 0, 0, 0, 0, width, height, mem);
	}
}


//
//	OtTextureClass::submit
//

void OtTextureClass::submit(int stage, bgfx::UniformHandle uniform) {
	bgfx::setTexture(stage, uniform, texture);
}


//
//	OtTextureClass::getMeta
//

OtType OtTextureClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtTextureClass>("Texture", OtGuiClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtTextureClass::init));
		type->set("loadImage", OtFunctionClass::create(&OtTextureClass::loadImage));
	}

	return type;
}


//
//	OtTextureClass::create
//

OtTexture OtTextureClass::create() {
	OtTexture texture = std::make_shared<OtTextureClass>();
	texture->setType(getMeta());
	return texture;
}
