//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
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
//	OtCubeMap::load
//

void OtCubeMap::load(
	const std::filesystem::path& posx,
	const std::filesystem::path& negx,
	const std::filesystem::path& posy,
	const std::filesystem::path& negy,
	const std::filesystem::path& posz,
	const std::filesystem::path& negz) {

	// load first side
	OtImage image;
	image.load(posx, true, true);
	bimg::ImageContainer* container = image.getContainer();
	uint16_t imageSize = container->m_width;
	bimg::TextureFormat::Enum imageFormat = container->m_format;

	// create a new cubemap
	cubemap = bgfx::createTextureCube(imageSize, false, 1, bgfx::TextureFormat::Enum(imageFormat), BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE);

	// store first side
	const bgfx::Memory* mem = bgfx::copy(container->m_data, container->m_size);
	bgfx::updateTextureCube(cubemap.getHandle(), 0, 0, 0, 0, 0, imageSize, imageSize, mem);

	// load and store other sides
	image.load(negx, true, true);
	container = image.getContainer();

	if (container->m_width != imageSize || container->m_format != imageFormat) {
		OtError("Cubemap image (negx] does not have same size or format as others");
	}

	mem = bgfx::copy(container->m_data, container->m_size);
	bgfx::updateTextureCube(cubemap.getHandle(), 0, 1, 0, 0, 0, imageSize, imageSize, mem);

	image.load(posy, true, true);
	container = image.getContainer();

	if (container->m_width != imageSize || container->m_format != imageFormat) {
		OtError("Cubemap image (posy] does not have same size or format as others");
	}

	mem = bgfx::copy(container->m_data, container->m_size);
	bgfx::updateTextureCube(cubemap.getHandle(), 0, 2, 0, 0, 0, imageSize, imageSize, mem);

	image.load(negy, true, true);
	container = image.getContainer();

	if (container->m_width != imageSize || container->m_format != imageFormat) {
		OtError("Cubemap image (negy] does not have same size or format as others");
	}

	mem = bgfx::copy(container->m_data, container->m_size);
	bgfx::updateTextureCube(cubemap.getHandle(), 0, 3, 0, 0, 0, imageSize, imageSize, mem);

	image.load(posz, true, true);
	container = image.getContainer();

	if (container->m_width != imageSize || container->m_format != imageFormat) {
		OtError("Cubemap image (posz] does not have same size or format as others");
	}

	mem = bgfx::copy(container->m_data, container->m_size);
	bgfx::updateTextureCube(cubemap.getHandle(), 0, 4, 0, 0, 0, imageSize, imageSize, mem);

	image.load(negz, true, true);
	container = image.getContainer();

	if (container->m_width != imageSize || container->m_format != imageFormat) {
		OtError("Cubemap image (negz] does not have same size or format as others");
	}

	mem = bgfx::copy(container->m_data, container->m_size);
	bgfx::updateTextureCube(cubemap.getHandle(), 0, 5, 0, 0, 0, imageSize, imageSize, mem);
}
