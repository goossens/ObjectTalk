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
//	OtTextureClass::setNoiseMap
//

OtObject OtTextureClass::setNoiseMap(OtObject object) {
	// ensure object is a noisemap
	if (object->isKindOf("NoiseMap")) {
		noisemap = object->cast<OtNoiseMapClass>();

	} else {
		OtExcept("Expected a [NoiseMap] object, not a [%s]", object->getType()->getName().c_str());
	}

	noisemap->attach([this]() {
		this->processNoiseMap();
	});

	processNoiseMap();
	return shared();
}


//
//	OtTextureClass::processNoiseMap
//

void OtTextureClass::processNoiseMap() {
	// get noisemap details
	size_t w = noisemap->getWidth();
	size_t h = noisemap->getHeight();
	float* noise = noisemap->getNoise();

	// create new image and texture if required
	if (image->m_width != w || image->m_height != h || image->m_format != bimg::TextureFormat::RGBA8) {
		bimg::imageFree(image);
		bgfx::destroy(texture);

		width = w;
		height = h;
		format = bimg::TextureFormat::RGBA8;

		image = bimg::imageAlloc(&allocator, format, width, height, 0, 1, false, false);
		texture = bgfx::createTexture2D(width, height, false, 1, bgfx::TextureFormat::Enum(format));
	}

	// update image and texture
	uint8_t* dest = (uint8_t*) image->m_data;
	float* src = noise;

	for (auto c = 0; c < width * height; c++) {
		uint8_t value = (uint8_t) (*src++ * 255.0);
		*dest++ = value;
		*dest++ = value;
		*dest++ = value;
		*dest++ = 255;
	}

	const bgfx::Memory* mem = bgfx::makeRef(image->m_data, image->m_size);
	bgfx::updateTexture2D(texture, 0, 0, 0, 0, width, height, mem);
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
		type->set("setNoiseMap", OtFunctionClass::create(&OtTextureClass::setNoiseMap));
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
