//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>

#include "bgfx/bgfx.h"
#include "bimg/decode.h"
#include "bx/file.h"

#include "ot/numbers.h"
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

static inline void packHeight(void* dst, const float* src) {
	float* d = (float*) dst;
	d[0] = 0.299 * src[0] + 0.587 * src[1] + 0.114 * src[2];
}


//
//	OtHeightMapClass::init
//

OtObject OtHeightMapClass::init(size_t count, OtObject* parameters) {
	switch (count) {
		case 2:
			scale = parameters[1]->operator float();

		case 1:
			loadMap(parameters[0]->operator std::string());

		case 0:
			break;

		default:
			OtExcept("[HeightMap] constructor expects up to 2 arguments (not %ld)", count);
	}

	return nullptr;
}


//
//	OtHeightMapClass::loadMap
//

OtObject OtHeightMapClass::loadMap(const std::string& file) {
	// delete old heightmap if required
	if (heightmap) {
		delete [] heightmap;
	}

	// load height map
	static bx::DefaultAllocator allocator;
	static bx::FileReader reader;

	if (!bx::open(&reader, file.c_str())) {
		OtExcept("Can't open heightmap [%s]", file.c_str());
	}

	uint32_t size = (uint32_t) bx::getSize(&reader);
	void* data = BX_ALLOC(&allocator, size);
	bx::read(&reader, data, size, bx::ErrorAssert{});
	bx::close(&reader);

	bimg::ImageContainer* image = bimg::imageParse(&allocator, data, size);

	if (!image)  {
		OtExcept("Can't process heightmap in [%s]", file.c_str());
	}

	// allocate heightmap
	width = image->m_width;
	height = image->m_height;
	heightmap = new float[width * height];

	// convert image to height map
	bimg::imageConvert(
		(void*) heightmap, sizeof(float) * 8, packHeight,
		image->m_data, bimg::getBitsPerPixel(image->m_format), bimg::getUnpack(image->m_format),
		uint32_t(width), uint32_t(height), 1,
		bimg::getBitsPerPixel(image->m_format) * width / 8, sizeof(float) * width);

	// free image resources
	BX_FREE(&allocator, data);
	bimg::imageFree(image);
	return shared();
}


//
//	OtHeightMapClass::setOffset
//

OtObject OtHeightMapClass::setOffset(float o) {
	offset = o;
	return shared();
}


//
//	OtHeightMapClass::setClamp
//

OtObject OtHeightMapClass::setClamp(float min, float max) {
	minClamp = min;
	maxClamp = max;
	return shared();
}


//
//	OtHeightMapClass::setScale
//

OtObject OtHeightMapClass::setScale(float s) {
	scale = s;
	return shared();
}


//
//	OtHeightMapClass::setHeightMap
//

void OtHeightMapClass::setHeightMap(int w, int h, float* values) {
	// delete old heightmap if required
	if (heightmap) {
		delete [] heightmap;
	}

	// remember size
	width = w;
	height = h;

	// save new map
	heightmap = new float[width * height];
	std::memcpy(heightmap, values, width * height * sizeof(float));
}


//
//	OtHeightMapClass::getHeightAbs
//

float OtHeightMapClass::getHeightAbs(int x, int y) {
	if (x < 0 || y < 0) {
		return 0.0;

	} else {
		x = x % width;
		y = y % height;
		return (std::clamp(heightmap[y * width + x], minClamp, maxClamp) + offset) * scale;
	}
}


//
//	OtHeightMapClass::getHeight
//

float OtHeightMapClass::getHeight(float x, float y) {
	x *= width - 1;
	y *= height - 1;

	int x1 = std::floor(x);
	int y1 = std::floor(y);
	int x2 = x + 1;
	int y2 = y + 1;

	auto h11 = getHeightAbs(x1, y1);
	auto h21 = getHeightAbs(x2, y1);
	auto h12 = getHeightAbs(x1, y2);
	auto h22 = getHeightAbs(x2, y2);

	auto hx1 = std::lerp(h11, h21, x - x1);
	auto hx2 = std::lerp(h12, h22, x - x1);
	return std::lerp(hx1, hx2, y - y1);
}


//
//	OtHeightMapClass::getNormal
//

glm::vec3 OtHeightMapClass::getNormal(float x, float y) {
	int ix = int(x * width);
	int iy = int(y * height);

	float heightL = getHeightAbs(ix - 1, iy);
	float heightR = getHeightAbs(ix + 1, iy);
	float heightU = getHeightAbs(ix, iy - 1);
	float heightD = getHeightAbs(ix, iy + 1);
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
		type->set("loadMap", OtFunctionClass::create(&OtHeightMapClass::loadMap));
		type->set("setClamp", OtFunctionClass::create(&OtHeightMapClass::setClamp));
		type->set("setOffset", OtFunctionClass::create(&OtHeightMapClass::setOffset));
		type->set("setScale", OtFunctionClass::create(&OtHeightMapClass::setScale));
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
