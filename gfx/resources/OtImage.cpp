//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>
#include <filesystem>
#include <fstream>

#include "bx/allocator.h"
#include "bimg/decode.h"

#include "OtException.h"
#include "OtNumbers.h"

#include "OtImage.h"
#include "OtFrameworkAtExit.h"


//
//	Globals
//

static bx::DefaultAllocator allocator;


//
//	OtImage::OtImage
//

OtImage::OtImage(const std::filesystem::path& path, bool powerof2, bool square) {
	load(path, powerof2, square);
}


//
//	OtImage::~OtImage
//

OtImage::~OtImage() {
	clear();
}


//
//	OtImage::clear
//

void OtImage::clear() {
	if (image) {
		bimg::imageFree(image);
		image = nullptr;
	}
}


//
//	OtImage::load
//

void OtImage::load(const std::filesystem::path& path, bool powerof2, bool square) {
	// clear old image (if required)
	clear();

	// get image data
	auto pathstring = path.string();
	auto filename = pathstring.c_str();

	if (!std::filesystem::exists(filename) || !std::filesystem::is_regular_file(filename)) {
		OtError("Can't open image in [%s]", filename);
	}

	auto filesize = std::filesystem::file_size(filename);
	auto buffer = new char[filesize];
	std::ifstream file(filename, std::ios::binary);
	file.read(buffer, filesize);

	if (!file) {
		delete [] buffer;
		OtError("Can't open image in [%s]", filename);
	}

	image = bimg::imageParse(&allocator, buffer, (uint32_t) filesize);
	delete [] buffer;

	if (!image) {
		OtError("Can't process image in [%s]", filename);
	}

	// validate sides are power of 2 (if required)
	if (powerof2 && !(bx::isPowerOf2(image->m_width))) {
		clear();
		OtError("Image width %d is not a power of 2", image->m_width);
	}

	if (powerof2 && !(bx::isPowerOf2(image->m_height))) {
		clear();
		OtError("Image height %d is not a power of 2", image->m_height);
	}

	// validate squareness (if required)
	if (square && image->m_width != image->m_height) {
		clear();
		OtError("Image must be square not %d by %d", image->m_width, image->m_height);
	}
}


//
//	OtImage::loadAsGrayscale
//

void OtImage::loadAsGrayscale(const std::filesystem::path& path, bool powerof2, bool square) {
	// load the image
	load(path, powerof2, square);

	// don't do anything if we are already in the right format
	if (image->m_format != bimg::TextureFormat::R32F) {
		// see if image is convertable
		if (!bimg::imageConvert(bimg::TextureFormat::R32F, image->m_format)) {
			OtError("Can't convert image in [%s] to grayscale", path.string().c_str());
		}

		// convert image
		bimg::ImageContainer* newImage = bimg::imageConvert(
			&allocator,
			bimg::TextureFormat::R32F,
			*image);

		// delete old image and use new one
		clear();
		image = newImage;
	}
}


//
//	OtImage::loadAsRGBA
//

void OtImage::loadAsRGBA(const std::filesystem::path& path, bool powerof2, bool square) {
	// load the image
	load(path, powerof2, square);

	// don't do anything if we are already in the right format
	if (image->m_format != bimg::TextureFormat::RGBA8) {
		// see if image is convertable
		if (!bimg::imageConvert(bimg::TextureFormat::RGBA8, image->m_format)) {
			OtError("Can't convert image in [%s] to grayscale", path.string().c_str());
		}

		// convert image
		bimg::ImageContainer* newImage = bimg::imageConvert(
			&allocator,
			bimg::TextureFormat::RGBA8,
			*image);

		// delete old image and use new one
		clear();
		image = newImage;
	}
}


//
//	OtImage::loadFromFileInMemory
//

void OtImage::loadFromFileInMemory(void* data, uint32_t size) {
	// clear old image (if required)
	clear();

	// create the image
	image = bimg::imageParse(&allocator, data, size);

	// check for errors
	if (!image) {
		OtError("Internal error: Can't process in-memory image");
	}
}


//
//	OtImage::getContainer
//

bimg::ImageContainer* OtImage::getContainer() {
	// ensure we have a valid image
	if (isValid()) {
		return image;

	} else {
		static bimg::ImageContainer* dummy = nullptr;

		// create dummy image (if required)
		if (!dummy) {
			static bx::DefaultAllocator allocator;
			dummy = bimg::imageAlloc(&allocator, bimg::TextureFormat::R8, 1, 1, 0, 1, false, false);

			OtFrameworkAtExit::instance()->add([] () {
				bimg::imageFree(dummy);
			});
		}

		// just return a dummy image to keep everybody happy
		return dummy;
	}
}


//
//	OtImage::getPixelRgba
//

glm::vec4 OtImage::getPixelRgba(size_t x, size_t y) {
	x = std::clamp(x, (size_t) 0, (size_t) (image->m_width - 1));
	y = std::clamp(y, (size_t) 0, (size_t) (image->m_height - 1));

	if (image->m_format == bimg::TextureFormat::R8) {
		auto value = ((float*) image->m_data)[y * image->m_width + x];
		return glm::vec4(value, value, value,1.0f);

	} else {
		auto value = &((uint8_t*) image->m_data)[y * image->m_width + x];
		return glm::vec4((float) value[0] / 255.0f, (float) value[1] / 255.0f, (float) value[2] / 255.0f, (float) value[3] / 255.0f);
	}
}


//
//	OtImage::getPixelGray
//

float OtImage::getPixelGray(size_t x, size_t y) {
	x = std::clamp(x, (size_t) 0, (size_t) image->m_width - 1);
	y = std::clamp(y, (size_t) 0, (size_t) image->m_height - 1);

	if (image->m_format == bimg::TextureFormat::R8) {
		return ((float*) image->m_data)[y * image->m_width + x];

	} else {
		auto value = &((uint8_t*) image->m_data)[y * image->m_width + x];
		return (float) value[0] / 255.0f * 0.3f + (float) value[1] / 255.0f * 0.59f + (float) value[2] / 255.0f * 0.11f;
	}
}


//
//	OtImage::sampleValue
//

float OtImage::sampleValue(float x, float y) {
	x *= image->m_width - 1;
	y *= image->m_height - 1;

	int x1 = std::floor(x);
	int y1 = std::floor(y);
	int x2 = x1 + 1;
	int y2 = y1 + 1;

	auto h11 = getPixelGray(x1, y1);
	auto h21 = getPixelGray(x2, y1);
	auto h12 = getPixelGray(x1, y2);
	auto h22 = getPixelGray(x2, y2);

	auto hx1 = std::lerp(h11, h21, x - x1);
	auto hx2 = std::lerp(h12, h22, x - x1);
	return std::lerp(hx1, hx2, y - y1);
}
