//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>
#include <cmath>
#include <cstring>
#include <ctime>

#include "imgui.h"

#include "OtException.h"
#include "OtNumbers.h"
#include "OtFunction.h"

#include "OtController.h"
#include "OtHeightMap.h"
#include "OtImage.h"


//
//	OtHeightMapClass::~OtHeightMapClass
//

OtHeightMapClass::~OtHeightMapClass() {
	clear();
}


//
//	OtHeightMapClass::clear
//

void OtHeightMapClass::clear() {
	if (heightmap) {
		delete [] heightmap;
		heightmap = nullptr;
	}
}


//
//	OtHeightMapClass::init
//

void OtHeightMapClass::init(size_t count, OtObject* parameters) {
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
}


//
//	OtHeightMapClass::loadMap
//

OtObject OtHeightMapClass::loadMap(const std::string& filename) {
	// delete old heightmap if required
	clear();

	// load height map from image
	OtImage image;
	image.loadAsGrayscale(filename);

	// allocate heightmap
	width = image.getWidth();
	height = image.getHeight();
	heightmap = new float[width * height];
	std::memcpy(heightmap, image.getPixels(), width * height * sizeof(float));

	// notify observers
	notify();
	return OtObject(this);
}


//
//	OtHeightMapClass::setScale
//

OtObject OtHeightMapClass::setScale(float s) {
	scale = s;
	notify();
	return OtObject(this);
}


//
//	OtHeightMapClass::setOffset
//

OtObject OtHeightMapClass::setOffset(float o) {
	offset = o;
	notify();
	return OtObject(this);
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
		type->set("__init__", OtFunction::create(&OtHeightMapClass::init));
		type->set("loadMap", OtFunction::create(&OtHeightMapClass::loadMap));
		type->set("setScale", OtFunction::create(&OtHeightMapClass::setScale));
		type->set("setOffset", OtFunction::create(&OtHeightMapClass::setOffset));
		type->set("getHeight", OtFunction::create(&OtHeightMapClass::getHeight));
	}

	return type;
}
