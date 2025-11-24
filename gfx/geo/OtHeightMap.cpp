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
#include <limits>

#include "OtAssert.h"
#include "OtNumbers.h"

#include "OtHeightMap.h"
#include "OtImage.h"


//
//	OtHeightMap::update
//

void OtHeightMap::update(int w, int h, bool clear) {
	// sanity checks
	OtAssert(w > 0);
	OtAssert(h > 0);

	// allocate heightmap (if required)
	if (!heightmap || width != w || height != h) {
		heightmap = std::make_unique<float[]>(w * h);
		width = w;
		height = h;
	}

	// clear values if required
	if (clear) {
		std::fill(heightmap.get(), heightmap.get() + w * h, 0.0f);
	}
}


//
//	OtHeightMap::load
//

void OtHeightMap::load(const std::string& path, float minValue, float maxValue) {
	// load height map from image
	OtImage image;
	image.load(path);

	// update heightmap (if required)
	update(image.getWidth(), image.getHeight());

	// copy pixels
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			auto pixel = image.getPixelGray(x, y);
			setHeight(x, y, std::lerp(minValue, maxValue, pixel));
		}
	}
}


//
//	OtHeightMap::setHeight
//

void OtHeightMap::setHeight(int x, int y, float value) {
	// sanity checks
	OtAssert(heightmap != nullptr);
	OtAssert(x >= 0 && x < width);
	OtAssert(y >= 0 && y < height);

	// set height
	heightmap[y * width + x] = value;
}


//
//	OtHeightMap::getHeight
//

float OtHeightMap::getHeight(int x, int y) {
	// sanity check
	OtAssert(heightmap != nullptr);

	// get height value
	x = std::clamp(x, 0, width - 1);
	y = std::clamp(y, 0, height - 1);
	return heightmap[y * width + x];
}


//
//	OtHeightMap::getHeight
//

float OtHeightMap::getHeight(float x, float y) {
	x *= width - 1;
	y *= height - 1;

	int x1 = static_cast<int>(std::floor(x));
	int y1 = static_cast<int>(std::floor(y));
	int x2 = x1 + 1;
	int y2 = y1 + 1;

	auto h11 = getHeight(x1, y1);
	auto h21 = getHeight(x2, y1);
	auto h12 = getHeight(x1, y2);
	auto h22 = getHeight(x2, y2);

	auto hx1 = std::lerp(h11, h21, x - x1);
	auto hx2 = std::lerp(h12, h22, x - x1);
	return std::lerp(hx1, hx2, y - y1);
}


//
//	OtHeightMap::getNormal
//

glm::vec3 OtHeightMap::getNormal(float x, float y) {
	int ix = int(x * width);
	int iy = int(y * height);

	return glm::normalize(glm::vec3(
		getHeight(ix - 1, iy) - getHeight(ix + 1, iy),
		2.0,
		getHeight(ix, iy - 1) - getHeight(ix, iy + 1)));
}


//
//	OtHeightMap::getMinMaxHeights
//

void OtHeightMap::getMinMaxHeights(float& minHeight, float& maxHeight) {
	minHeight = std::numeric_limits<float>::max();
	maxHeight = std::numeric_limits<float>::lowest();

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			auto h = getHeight(x, y);
			minHeight = std::min(minHeight, h);
			maxHeight = std::max(maxHeight, h);
		}
	}
}
