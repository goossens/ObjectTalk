//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "bx/allocator.h"
#include "bx/file.h"
#include "bimg/decode.h"

#include "OtException.h"

#include "OtImage.h"
#include "OtFramework.h"


//
//	Globals
//

static bx::DefaultAllocator allocator;
static bx::FileReader reader;


//
//	OtImage::OtImage
//

OtImage::OtImage(const std::string &file, bool powerof2, bool square) {
	load(file, powerof2, square);
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

void OtImage::load(const std::string &file, bool powerof2, bool square) {
	// clear old image (if required)
	clear();

	// get image data
	if (!bx::open(&reader, file.c_str())) {
		OtExcept("Can't open image [%s]", file.c_str());
	}

	uint32_t size = (uint32_t) bx::getSize(&reader);
	void* data = BX_ALLOC(&allocator, size);
	bx::read(&reader, data, size, bx::ErrorAssert{});
	bx::close(&reader);

	image = bimg::imageParse(&allocator, data, size);
	BX_FREE(&allocator, data);

	if (!image) {
		OtExcept("Can't process image in [%s]", file.c_str());
	}

	// validate sides are power of 2 (if required)
	if (powerof2 && !(bx::isPowerOf2(image->m_width))) {
		clear();
		OtExcept("Image width %d is not a power of 2", image->m_width);
	}

	if (powerof2 && !(bx::isPowerOf2(image->m_height))) {
		clear();
		OtExcept("Image height %d is not a power of 2", image->m_height);
	}

	// validate squareness (if required)
	if (square && image->m_width != image->m_height) {
		clear();
		OtExcept("Image must be square not %d by %d", image->m_width, image->m_height);
	}
}


//
//	OtImage::loadAsGrayscale
//

void OtImage::loadAsGrayscale(const std::string &file, bool powerof2, bool square) {
	// load the image
	load(file, powerof2, square);

	// don't do anything if we are already in the right format
	if (image->m_format != bimg::TextureFormat::R32F) {
		// see if image is convertable
		if (!bimg::imageConvert(bimg::TextureFormat::R32F, image->m_format)) {
			OtExcept("Can't convert image in [%s] to grayscale", file.c_str());
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

void OtImage::loadAsRGBA(const std::string &file, bool powerof2, bool square) {
	// load the image
	load(file, powerof2, square);

	// don't do anything if we are already in the right format
	if (image->m_format != bimg::TextureFormat::RGBA8) {
		// see if image is convertable
		if (!bimg::imageConvert(bimg::TextureFormat::RGBA8, image->m_format)) {
			OtExcept("Can't convert image in [%s] to grayscale", file.c_str());
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
//	OtImage::load
//

void OtImage::load(void *data, uint32_t size) {
	// clear old image (if required)
	clear();

	// create the image
	image = bimg::imageParse(&allocator, data, size);

	// check for errors
	if (!image) {
		OtExcept("Internal error: Can't process in-memory image");
	}
}


//
//	OtImage::getContainer
//

bimg::ImageContainer *OtImage::getContainer() {
	// unsure we have a valid image
	if (isValid()) {
		return image;

	} else {
		static bimg::ImageContainer* dummy = nullptr;

		// create dummy image (if required)
		if (!dummy) {
			static bx::DefaultAllocator allocator;
			dummy = bimg::imageAlloc(&allocator, bimg::TextureFormat::R8, 1, 1, 0, 1, false, false);

			OtFrameworkClass::instance()->atexit([] () {
				bimg::imageFree(dummy);
			});
		}

		// just return a dummy image to keep everybody happy
		return dummy;
	}
}
