//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <exception>
#include <fstream>
#include <sstream>

#include "nlohmann/json.hpp"

#include "OtException.h"

#include "OtCubeMap.h"
#include "OtImage.h"


//
//	OtCubeMap::load
//

void OtCubeMap::load(const std::filesystem::path& path) {
	auto ext = path.extension();

	if (ext == ".cubemap") {
		loadJSON(path);

	} else {
		loadCubemapImage(path);
	}
}


//
//	getPath
//

static inline std::filesystem::path getPath(nlohmann::json data, const char* field, const std::filesystem::path& basedir) {
	// make a path absolute based on a provided base directory
	if (data.contains(field)) {
		std::string value = data[field];

		if (value.size()) {
			return basedir / std::filesystem::path(value);

		} else {
			return std::filesystem::path();
		}

	} else {
		return std::filesystem::path();
	}
}


//
//	OtCubeMap::loadJSON
//

void OtCubeMap::loadJSON(const std::filesystem::path& path) {
	// load cubemap definition from file
	std::stringstream buffer;

	try {
		std::ifstream stream(path.string().c_str());

		if (stream.fail()) {
			OtError("Can't read from file [%s]", path.string().c_str());
		}

		buffer << stream.rdbuf();
		stream.close();

	} catch (std::exception& e) {
		OtError("Can't read from file [%s], error: %s", path.string().c_str(), e.what());
	}

	// parse json
	auto basedir = path.parent_path();
	auto data = nlohmann::json::parse(buffer.str());

	auto negx = getPath(data, "negx", basedir);
	auto negy = getPath(data, "negy", basedir);
	auto negz = getPath(data, "negz", basedir);
	auto posx = getPath(data, "posx", basedir);
	auto posy = getPath(data, "posy", basedir);
	auto posz = getPath(data, "posz", basedir);

	if (negx.empty() || negy.empty() || negz.empty() || posx.empty() || posy.empty() || posz.empty()) {
		OtError("Incomplete CubeMap specification in [%s]", path.string().c_str());
	}

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


//
//	OtCubeMap::loadCubemapImage
//

void OtCubeMap::loadCubemapImage(const std::filesystem::path& path) {
	// load image
	OtImage image;
	image.load(path, true, true);
	bimg::ImageContainer* container = image.getContainer();

	// sanity check
	if (!container->m_cubeMap) {
		OtError("Image [%s] is not a Cube Map", path.string().c_str());
	}

	// create a new cubemap
	cubemap = bgfx::createTextureCube(
		container->m_width,
		false,
		1,
		bgfx::TextureFormat::Enum(container->m_format),
		BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE,
		bgfx::copy(container->m_data, container->m_size));
}
