//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>
#include <cmath>
#include <cstring>

#include "OtNumbers.h"

#include "OtHeightMap.h"
#include "OtImage.h"


//
//	OtHeightMap::load
//

void OtHeightMap::load(const std::string& path) {
	// load height map from image
	OtImage image;
	image.loadAsGrayscale(path);

	// allocate heightmap
	width = (int) image.getWidth();
	height = (int) image.getHeight();
	heightmap = std::make_unique<float[]>(width * height);
	std::memcpy(heightmap.get(), image.getPixels(), width * height * sizeof(float));
}


//
//	OtHeightMap::getHeightAbs
//

float OtHeightMap::getHeightAbs(int x, int y) {
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
//	OtHeightMap::getNormal
//

glm::vec3 OtHeightMap::getNormal(float x, float y) {
	int ix = int(x * width);
	int iy = int(y * height);

	return glm::normalize(glm::vec3(
		getHeightAbs(ix - 1, iy) - getHeightAbs(ix + 1, iy),
		2.0,
		getHeightAbs(ix, iy - 1) - getHeightAbs(ix, iy + 1)));
}
