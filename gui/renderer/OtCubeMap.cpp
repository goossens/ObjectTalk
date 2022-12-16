//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtException.h"

#include "OtCubeMap.h"
#include "OtImage.h"


//
//	OtCubeMap::OtCubeMap
//

OtCubeMap::OtCubeMap(const std::string &filename) {
}


//
//	OtCubeMap::~OtCubeMap
//

OtCubeMap::~OtCubeMap() {
	clear();
}


//
//	OtCubeMap::load
//

void OtCubeMap::load(const std::string &posx, const std::string &negx, const std::string &posy, const std::string &negy, const std::string &posz, const std::string &negz) {
	// cleanup (if required)
	if (bgfx::isValid(cubemap)) {
		bgfx::destroy(cubemap);
	}

	// load first side
	bimg::ImageContainer* image = OtImage(posx, true, true).getContainer();
	uint16_t imageSize = image->m_width;
	bimg::TextureFormat::Enum imageFormat = image->m_format;

	// create a new cubemap
	cubemap = bgfx::createTextureCube(imageSize, false, 1, bgfx::TextureFormat::Enum(imageFormat), BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE);

	// store first side
	const bgfx::Memory* mem = bgfx::copy(image->m_data, image->m_size);
	bgfx::updateTextureCube(cubemap, 0, 0, 0, 0, 0, imageSize, imageSize, mem);

	// load and store other sides
	image = OtImage(negx, true, true).getContainer();

	if (image->m_width != imageSize || image->m_format != imageFormat) {
		OtExcept("Cubemap image (negx] does not have same size or format as others");
	}

	mem = bgfx::copy(image->m_data, image->m_size);
	bgfx::updateTextureCube(cubemap, 0, 1, 0, 0, 0, imageSize, imageSize, mem);

	image = OtImage(posy, true, true).getContainer();

	if (image->m_width != imageSize || image->m_format != imageFormat) {
		OtExcept("Cubemap image (posy] does not have same size or format as others");
	}

	mem = bgfx::copy(image->m_data, image->m_size);
	bgfx::updateTextureCube(cubemap, 0, 2, 0, 0, 0, imageSize, imageSize, mem);

	image = OtImage(negy, true, true).getContainer();

	if (image->m_width != imageSize || image->m_format != imageFormat) {
		OtExcept("Cubemap image (negy] does not have same size or format as others");
	}

	mem = bgfx::copy(image->m_data, image->m_size);
	bgfx::updateTextureCube(cubemap, 0, 3, 0, 0, 0, imageSize, imageSize, mem);

	image = OtImage(posz, true, true).getContainer();

	if (image->m_width != imageSize || image->m_format != imageFormat) {
		OtExcept("Cubemap image (posz] does not have same size or format as others");
	}

	mem = bgfx::copy(image->m_data, image->m_size);
	bgfx::updateTextureCube(cubemap, 0, 4, 0, 0, 0, imageSize, imageSize, mem);

	image = OtImage(negz, true, true).getContainer();

	if (image->m_width != imageSize || image->m_format != imageFormat) {
		OtExcept("Cubemap image (negz] does not have same size or format as others");
	}

	mem = bgfx::copy(image->m_data, image->m_size);
	bgfx::updateTextureCube(cubemap, 0, 5, 0, 0, 0, imageSize, imageSize, mem);
}


//
//	OtCubeMap::clear
//

void OtCubeMap::clear() {
	if (bgfx::isValid(cubemap)) {
		bgfx::destroy(cubemap);
		cubemap = BGFX_INVALID_HANDLE;
	}
}
