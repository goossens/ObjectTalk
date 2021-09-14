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

#include "texture.h"


//
//	OtTextureClass::~OtTextureClass
//

OtTextureClass::~OtTextureClass() {
	// release resources
	if (image) {
		bimg::imageFree(image);
	}

	if (bgfx::isValid(texture)) {
		bgfx::destroy(texture);
	}
}


//
//	OtTextureClass::init
//

void OtTextureClass::init(const std::string& file) {
	// load named texture
	static bx::DefaultAllocator allocator;
	static bx::FileReader reader;

	if (!bx::open(&reader, file.c_str())) {
		OtExcept("Can't open texture [%s]", file.c_str());
	}

	uint32_t size = (uint32_t) bx::getSize(&reader);
	void* data = BX_ALLOC(&allocator, size);
	bx::read(&reader, data, size);
	bx::close(&reader);

	image = bimg::imageParse(&allocator, data, size);

	if (!image)  {
		OtExcept("Can't process texture in [%s]", file.c_str());
	}

	const bgfx::Memory* mem = bgfx::makeRef(image->m_data, image->m_size);
	BX_FREE(&allocator, data);

	if (!bgfx::isTextureValid(0, false, image->m_numLayers, bgfx::TextureFormat::Enum(image->m_format), BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE)) {
		OtExcept("Invalid texture format in [%s]", file.c_str());
	}

	texture = bgfx::createTexture2D(
		uint16_t(image->m_width),
		uint16_t(image->m_height),
		1 < image->m_numMips,
		image->m_numLayers,
		bgfx::TextureFormat::Enum(image->m_format),
		BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE,
		mem);
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
