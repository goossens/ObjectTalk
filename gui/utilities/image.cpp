//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/exception.h"

#include "image.h"


//
//	isPowerOf2
//

bool isPowerOf2(int number) {
	return (number != 0) && ((number & (number - 1)) == 0);
}


//
//	OtLoadImage
//

bimg::ImageContainer* OtLoadImage(const std::string& file, bool powerof2, bool square) {
	// load named image
	static bx::DefaultAllocator allocator;
	static bx::FileReader reader;

	if (!bx::open(&reader, file.c_str())) {
		OtExcept("Can't open image [%s]", file.c_str());
	}

	uint32_t size = (uint32_t) bx::getSize(&reader);
	void* data = BX_ALLOC(&allocator, size);
	bx::read(&reader, data, size, bx::ErrorAssert{});
	bx::close(&reader);

	bimg::ImageContainer* image = bimg::imageParse(&allocator, data, size);
	BX_FREE(&allocator, data);

	if (!image)  {
		OtExcept("Can't process image in [%s]", file.c_str());
	}

	// validate sides are power of 2 (if required)
	if (powerof2 && !(isPowerOf2(image->m_width))) {
		bimg::imageFree(image);
		OtExcept("Image width %d is not a power of 2", image->m_width);
	}

	if (powerof2 && !(isPowerOf2(image->m_height))) {
		bimg::imageFree(image);
		OtExcept("Image height %d is not a power of 2", image->m_height);
	}

	// validate aquareness (if required)
	if (square && image->m_width != image->m_height) {
		bimg::imageFree(image);
		OtExcept("Image must be square not %d by %d", image->m_width, image->m_height);
	}

	return image;
}
