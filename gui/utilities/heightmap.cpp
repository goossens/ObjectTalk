//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
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

#include "ot/numbers.h"
#include "ot/function.h"

#include "controller.h"
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

	// notify observers
	notify();
	return shared();
}


//
//	OtHeightMapClass::setScale
//

OtObject OtHeightMapClass::setScale(float s) {
	scale = s;
	notify();
	return shared();
}


//
//	OtHeightMapClass::setOffset
//

OtObject OtHeightMapClass::setOffset(float o) {
	offset = o;
	notify();
	return shared();
}


//
//	OtHeightMapClass::getHeightAbs
//

float OtHeightMapClass::getHeightAbs(int x, int y) {
	x = std::clamp(x, 0, width - 1);
	y = std::clamp(y, 0, height - 1);
	return heightmap[y * width + x] * scale + offset;
}


//
//	OtHeightMapClass::getHeight
//

float OtHeightMapClass::getHeight(float x, float y) {
	x *= width - 1;
	y *= height - 1;

	int x1 = std::floor(x);
	int y1 = std::floor(y);
	int x2 = x1 + 1;
	int y2 = y1 + 1;

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

	return glm::normalize(glm::vec3(
		getHeightAbs(ix - 1, iy) - getHeightAbs(ix + 1, iy),
		2.0,
		getHeightAbs(ix, iy - 1) - getHeightAbs(ix, iy + 1)));
}


//
//	OtHeightMapClass::renderGUI
//

void OtHeightMapClass::renderGUI() {
	bool changed = false;

	if (ImGui::SliderFloat("Scale", &scale, 1.0f, 100.0f)) {
		changed = true;
	}

	if (ImGui::SliderFloat("Ofsset", &offset, -50.0f, 50.0f)) {
		changed = true;
	}

	if (changed) {
		notify();
	}
}


//
//	OtHeightMapClass::getMeta
//

OtType OtHeightMapClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtHeightMapClass>("HeightMap", OtGuiClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtHeightMapClass::init));
		type->set("loadMap", OtFunctionClass::create(&OtHeightMapClass::loadMap));
		type->set("setScale", OtFunctionClass::create(&OtHeightMapClass::setScale));
		type->set("setOffset", OtFunctionClass::create(&OtHeightMapClass::setOffset));
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
