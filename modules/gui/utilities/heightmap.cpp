//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "bgfx/bgfx.h"
#include "bimg/decode.h"
#include "bx/file.h"

#include "ot/function.h"

#include "heightmap.h"


//
//	OtHeightMapClass::~OtHeightMapClass
//

OtHeightMapClass::~OtHeightMapClass() {
	if (heightmap) {
		delete [] heightmap;
	}
}


//
//	packHeight
//

static inline void packHeight(void* _dst, const float* _src) {
	double* dst = (double*) _dst;
	dst[0] = 0.299 * _src[0] + 0.587 * _src[1] + 0.114 * _src[2];
}


//
//	OtHeightMapClass::init
//

void OtHeightMapClass::init(const std::string& file, double s) {
	// save scale
	scale = s;

	// load height map
	static bx::DefaultAllocator allocator;
	static bx::FileReader reader;

	if (!bx::open(&reader, file.c_str())) {
		OtExcept("Can't open heightmap [%s]", file.c_str());
	}

	uint32_t size = (uint32_t) bx::getSize(&reader);
	void* data = BX_ALLOC(&allocator, size);
	bx::read(&reader, data, size);
	bx::close(&reader);

	bimg::ImageContainer* image = bimg::imageParse(&allocator, data, size);

	if (!image)  {
		OtExcept("Can't process heightmap in [%s]", file.c_str());
	}

	// allocate heightmap
	width = image->m_width;
	height = image->m_height;
	heightmap = new double[width * height];

	// convert image to height map
	bimg::imageConvert(
		(void*) heightmap, sizeof(double) * 8, packHeight,
		image->m_data, bimg::getBitsPerPixel(image->m_format), bimg::getUnpack(image->m_format),
		uint32_t(width), uint32_t(height), 1,
		bimg::getBitsPerPixel(image->m_format) * width / 8, sizeof(double) * width);

	// free image resources
	BX_FREE(&allocator, data);
	bimg::imageFree(image);
}


//
//	OtHeightMapClass::getHeightAbs
//

double OtHeightMapClass::getHeightAbs(int x, int y) {
	if (x < 0 || x >= width || y < 0 || y >= height) {
		return 0.0;

	} else {
		return heightmap[y * width + x] * scale;
	}
}


//
//	OtHeightMapClass::getHeight
//

double OtHeightMapClass::getHeight(double x, double y) {
	return getHeightAbs(int(x * width), int(y * height));
}


//
//	OtHeightMapClass::getNormal
//

glm::vec3 OtHeightMapClass::getNormal(double x, double y) {
	int ix = int(x * width);
	int iy = int(y * height);

	double heightL = getHeightAbs(ix - 1, iy);
	double heightR = getHeightAbs(ix + 1, iy);
	double heightU = getHeightAbs(ix, iy - 1);
	double heightD = getHeightAbs(ix, iy + 1);
	return glm::normalize(glm::vec3(heightL - heightR, 2, heightD - heightU));
}


//
//	OtHeightMapClass::getMeta
//

OtType OtHeightMapClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtHeightMapClass>("HeightMap", OtGuiClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtHeightMapClass::init));
		type->set("getHeight", OtFunctionClass::create(&OtHeightMapClass::getHeight));
	}

	return type;
}


//
//	OtHeightMapClass::create
//

OtHeightMap OtHeightMapClass::create() {
	OtHeightMap heightmap = std::make_shared<OtHeightMapClass>();
	heightmap->setType(getMeta());
	return heightmap;
}
