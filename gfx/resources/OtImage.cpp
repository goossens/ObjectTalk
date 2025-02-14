//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>
#include <fstream>

#include "bx/allocator.h"
#include <bx/file.h>
#include "bimg/decode.h"
#include "bimg/encode.h"

#include "OtAssert.h"
#include "OtException.h"
#include "OtLog.h"
#include "OtNumbers.h"

#include "OtImage.h"
#include "OtFrameworkAtExit.h"
#include "OtPath.h"


//
//	Globals
//

static bx::DefaultAllocator allocator;


//
//	OtImage::OtImage
//

OtImage::OtImage(const std::string& path, bool powerof2, bool square) {
	load(path, powerof2, square);
}


//
//	OtImage::clear
//

void OtImage::clear() {
	image.reset();
	incrementVersion();
}


//
//	OtImage::update
//

void OtImage::update(int width, int height, int format) {
	if (!image || image->m_width != width || image->m_height != height || image->m_format != format) {
		// allocate space for a new image
		auto imageContainer = bimg::imageAlloc(
			&allocator,
			bimg::TextureFormat::Enum(format),
			uint16_t(width),
			uint16_t(height),
			1,
			1,
			false,
			false);

		if (!imageContainer) {
			OtLogFatal("Function [bimg::imageAlloc] failed");
		}

		assignImageContainer(imageContainer);
	}
}


//
//	OtImage::load
//

void OtImage::load(const std::string& path, bool powerof2, bool square) {
	// get image data
	if (!OtPath::exists(path) || !OtPath::isRegularFile(path)) {
		OtError("Can't open image in [{}]", path);
	}

	auto filesize = OtPath::getFileSize(path);
	auto buffer = new char[filesize];
	std::ifstream file(path, std::ios::binary);
	file.read(buffer, filesize);

	if (!file) {
		delete [] buffer;
		OtError("Can't open image in [{}]", path);
	}

	auto imageContainer = bimg::imageParse(&allocator, buffer, (uint32_t) filesize);
	delete [] buffer;

	if (!imageContainer) {
		OtLogFatal("Function [bimg::imageParse] failed");
	}

	if (!imageContainer) {
		OtError("Can't process image in [{}]", path);
	}

	// validate sides are power of 2 (if required)
	if (powerof2 && !(bx::isPowerOf2(imageContainer->m_width))) {
		int width = imageContainer->m_width;
		bimg::imageFree(imageContainer);
		OtError("Image width {} is not a power of 2", width);
	}

	if (powerof2 && !(bx::isPowerOf2(imageContainer->m_height))) {
		int height = imageContainer->m_height;
		bimg::imageFree(imageContainer);
		OtError("Image height {} is not a power of 2", height);
	}

	// validate squareness (if required)
	if (square && imageContainer->m_width != imageContainer->m_height) {
		int width = imageContainer->m_width;
		int height = imageContainer->m_height;
		bimg::imageFree(imageContainer);
		OtError("Image must be square not {} by {}", width, height);
	}

	assignImageContainer(imageContainer);
}


//
//	convertImage
//

static bimg::ImageContainer* convertImage(bimg::ImageContainer& input, bimg::TextureFormat::Enum format) {
	if (!bimg::imageConvert(format, input.m_format)) {
		OtLogFatal("Can't convert image from [{}] to [{}]", bimg::getName(input.m_format), bimg::getName(format));
	}

	return bimg::imageConvert(&allocator, format, input);
}


//
//	OtImage::loadAsGrayscale
//

void OtImage::loadAsGrayscale(const std::string& path, bool powerof2, bool square) {
	// load the image
	load(path, powerof2, square);

	// convert image (if required)
	if (image->m_format != bimg::TextureFormat::R32F) {
		convertImage(*image, bimg::TextureFormat::R32F);
	}
}


//
//	OtImage::loadAsRGBA
//

void OtImage::loadAsRGBA(const std::string& path, bool powerof2, bool square) {
	// load the image
	load(path, powerof2, square);

	// convert image (if required)
	if (image->m_format != bimg::TextureFormat::RGBA8) {
		convertImage(*image, bimg::TextureFormat::RGBA8);
	}
}


//
//	OtImage::loadFromFileInMemory
//

void OtImage::loadFromFileInMemory(void* data, uint32_t size) {
	// create the image
	auto imageContainer = bimg::imageParse(&allocator, data, size);

	// check for errors
	if (!imageContainer) {
		OtError("Internal error: can't process in-memory image");
	}

	assignImageContainer(imageContainer);
}


//
//	writeToPNG
//

static void writeToPNG(const std::string& path, const bimg::ImageContainer& image) {
	bx::FileWriter writer;
	bx::Error err;

	if (bx::open(&writer, path.c_str(), false, &err)) {
		bimg::imageWritePng(
			&writer,
			image.m_width,
			image.m_height,
			image.m_width * (image.m_format == bimg::TextureFormat::R8 ? 1 : 4),
			image.m_data,
			image.m_format,
			false,
			&err);

		if (!err.isOk()) {
			auto msg = err.getMessage();
			auto message = std::string(msg.getCPtr(), msg.getLength());
			OtError("Can't write PNG to file at [{}], error: {}", path, message);
		}

		bx::close(&writer);;

	} else {
		auto msg = err.getMessage();
		auto message = std::string(msg.getCPtr(), msg.getLength());
		OtError("Can't open PNG file at [{}] for write, error: {}", path, message);
	}
}


//
//	OtImage::saveToPNG
//

void OtImage::saveToPNG(const std::string& path) {
	// sanity check
	OtAssert(image);

	if (image->m_format == r8Image) {
		writeToPNG(path, *image);

	} else if (image->m_format == r16Image || image->m_format == rFloat32Image) {
		bimg::ImageContainer* newImage = convertImage(*image, bimg::TextureFormat::R8);
		writeToPNG(path, *newImage);
		bimg::imageFree(newImage);

	} else if (image->m_format == rgba8Image) {
		writeToPNG(path, *image);

	} else {
		bimg::ImageContainer* newImage = convertImage(*image, bimg::TextureFormat::RGBA8);
		writeToPNG(path, *newImage);
		bimg::imageFree(newImage);
	}
}


//
//	writeToDDS
//

static void writeToDDS(const std::string& path, bimg::ImageContainer& image) {
	bx::FileWriter writer;
	bx::Error err;

	if (bx::open(&writer, path.c_str(), false, &err)) {
		bimg::imageWriteDds(
			&writer,
			image,
			image.m_data,
			image.m_size,
			&err);

		if (!err.isOk()) {
			auto msg = err.getMessage();
			auto message = std::string(msg.getCPtr(), msg.getLength());
			OtError("Can't write DDS to file at [{}], error: {}", path, message);
		}

		bx::close(&writer);;

	} else {
		auto msg = err.getMessage();
		auto message = std::string(msg.getCPtr(), msg.getLength());
		OtError("Can't open DDS file at [{}] for write, error: {}", path, message);
	}
}


//
//	OtImage::saveToDDS
//

void OtImage::saveToDDS(const std::string& path) {
	// sanity check
	OtAssert(image);

	if (image->m_format == r8Image) {
		writeToDDS(path, *image);

	} else if (image->m_format == r16Image) {
		bimg::ImageContainer* newImage = convertImage(*image, bimg::TextureFormat::R16U);
		writeToDDS(path, *newImage);
		bimg::imageFree(newImage);

	} else if (image->m_format == rFloat32Image) {
		bimg::ImageContainer* newImage = convertImage(*image, bimg::TextureFormat::R32U);
		writeToDDS(path, *newImage);
		bimg::imageFree(newImage);

	} else if (image->m_format == rgba8Image) {
		writeToDDS(path, *image);

	} else {
		bimg::ImageContainer* newImage = convertImage(*image, bimg::TextureFormat::RGBA8);
		writeToDDS(path, *newImage);
		bimg::imageFree(newImage);
	}
}


//
//	OtImage::getContainer
//

bimg::ImageContainer* OtImage::getContainer() {
	// ensure we have a valid image
	if (isValid()) {
		return image.get();

	} else {
		static bimg::ImageContainer* dummy = nullptr;

		// create dummy image (if required)
		if (!dummy) {
			dummy = bimg::imageAlloc(&allocator, bimg::TextureFormat::R8, 1, 1, 0, 1, false, false);

			OtFrameworkAtExit::add([]() {
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

glm::vec4 OtImage::getPixelRgba(int x, int y) {
	// sanity check
	OtAssert(isValid());

	x = std::clamp(x, 0, (int) (image->m_width - 1));
	y = std::clamp(y, 0, (int) (image->m_height - 1));

	if (image->m_format == bimg::TextureFormat::R8) {
		auto value = ((uint8_t*) image->m_data)[y * image->m_width + x] / 256.0f;
		return glm::vec4(value, value, value, 1.0f);

	} else if (image->m_format == bimg::TextureFormat::R32F) {
		auto value = ((float*) image->m_data)[y * image->m_width + x];
		return glm::vec4(value, value, value, 1.0f);

	} else if (image->m_format == bimg::TextureFormat::RGBA8) {
		auto value = &((uint8_t*) image->m_data)[y * image->m_width + x];
		return glm::vec4(value[0] / 255.0f, value[1] / 255.0f, value[2] / 255.0f, value[3] / 255.0f);

	} else if (image->m_format == bimg::TextureFormat::RGBA32F) {
		auto value = &((float*) image->m_data)[y * image->m_width + x];
		return glm::vec4(value[0], value[1], value[2], value[3]);

	} else {
		OtLogFatal("Can't get pixel from image in [{}] format", bimg::getName(image->m_format));
		return glm::vec4();
	}
}


//
//	OtImage::getPixelGray
//

float OtImage::getPixelGray(int x, int y) {
	// sanity check
	OtAssert(isValid());

	x = std::clamp(x, 0, (int) image->m_width - 1);
	y = std::clamp(y, 0, (int) image->m_height - 1);

	if (image->m_format == bimg::TextureFormat::R8) {
		return ((uint8_t*) image->m_data)[y * image->m_width + x] / 256.0f;

	} else if (image->m_format == bimg::TextureFormat::R32F) {
		return ((float*) image->m_data)[y * image->m_width + x];

	} else if (image->m_format == bimg::TextureFormat::RGBA8) {
		auto value = &((uint8_t*) image->m_data)[(y * image->m_width + x) * 4];
		return value[0] / 255.0f * 0.3f + value[1] / 255.0f * 0.59f + value[2] / 255.0f * 0.11f;

	} else if (image->m_format == bimg::TextureFormat::RGBA32F) {
		auto value = &((float*) image->m_data)[(y * image->m_width + x) * 4];
		return value[0] * 0.3f + value[1] * 0.59f + value[2] * 0.11f;

	} else {
		OtLogFatal("Can't get pixel from image in [{}] format", bimg::getName(image->m_format));
		return 0.0f;
	}
}


//
//	OtImage::sampleValueRgba
//

glm::vec4 OtImage::sampleValueRgba(float x, float y) {
	// sanity check
	OtAssert(isValid());

	x *= image->m_width - 1;
	y *= image->m_height - 1;

	int x1 = std::floor(x);
	int y1 = std::floor(y);
	int x2 = x1 + 1;
	int y2 = y1 + 1;

	auto h11 = getPixelRgba(x1, y1);
	auto h21 = getPixelRgba(x2, y1);
	auto h12 = getPixelRgba(x1, y2);
	auto h22 = getPixelRgba(x2, y2);

	auto hx1 = glm::mix(h11, h21, x - x1);
	auto hx2 = glm::mix(h12, h22, x - x1);
	return glm::mix(hx1, hx2, y - y1);
}


//
//	OtImage::sampleValueGray
//

float OtImage::sampleValueGray(float x, float y) {
	// sanity check
	OtAssert(isValid());

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


//
//	OtImage::assignImageContainer
//

void OtImage::assignImageContainer(bimg::ImageContainer* container) {
	// the deleter for an image container
	struct Deleter {
		void operator()(bimg::ImageContainer* image) {
			bimg::imageFree(image);
		}
	} deleter;

	// assign a new image container to the shared pointer
	image.reset(container, deleter);
}
