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
#include <cstdio>
#include <cstring>
#include <limits>
#include <numbers>

#include "tiffio.h"

#include "OtAssert.h"
#include "OtLog.h"

#include "OtHeightMap.h"


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
//	errorHandler
//

static int errorHandler(TIFF*, void* data, const char*, const char* format, va_list args) {
	auto heightmap = static_cast<OtHeightMap*>(data);
	char buffer[512];
	vsnprintf(buffer, sizeof(buffer), format, args);
	heightmap->setErrorMessage(buffer);
	return 1;
}


//
//	OtHeightMap::load
//

void OtHeightMap::load(const std::string& path) {
	// open file for read
	auto options = TIFFOpenOptionsAlloc();
	TIFFOpenOptionsSetErrorHandlerExtR(options, errorHandler, this);
	auto tiff = TIFFOpenExt(path.c_str(), "r", options);
	TIFFOpenOptionsFree(options);

	if (!tiff) {
		OtLogError("Can't load from heightmap file [{}]: {}", path, errorMessage);
	}

	// sanity checks
	uint32_t rows;
	uint32_t columns;
	uint16_t sampleFormat;
	uint16_t componentsPerPixel;

	if (TIFFGetField(tiff, TIFFTAG_IMAGELENGTH, &rows) != 1) { OtLogError("[TIFFTAG_IMAGELENGTH] failed: {}", errorMessage); }
	if (TIFFGetField(tiff, TIFFTAG_IMAGEWIDTH, &columns) != 1) { OtLogError("[TIFFTAG_IMAGEWIDTH] failed: {}", errorMessage); }
	if (TIFFGetField(tiff, TIFFTAG_SAMPLEFORMAT, &sampleFormat) != 1) { OtLogError("[TIFFTAG_SAMPLEFORMAT] failed: {}", errorMessage); }
	if (TIFFGetField(tiff, TIFFTAG_SAMPLESPERPIXEL, &componentsPerPixel) != 1) { OtLogError("[TIFFTAG_SAMPLESPERPIXEL] failed: {}", errorMessage); }

	if (rows < 1) {
		OtLogError("Invalid TIFF file [{}]: height [{}] < 1", path, rows);
	}

	if (columns < 1) {
		OtLogError("Invalid TIFF file [{}]: width [{}] < 1", path, rows);
	}

	if (componentsPerPixel != 1) {
		OtLogError("Unsupported TIFF file [{}]: too many components per pixel [{}]", path, componentsPerPixel);
	}

	if (sampleFormat == SAMPLEFORMAT_IEEEFP) {
		update(static_cast<int>(columns), static_cast<int>(rows));
		auto p = heightmap.get();

		for (uint32_t row = 0; row < rows; row++) {
			if (TIFFReadScanline(tiff, p, row) != 1) {
				OtLogError("Error while reading from TIFF file [{}]: {}", path, errorMessage);
			}

			p += columns;
		}

	} else {
		OtLogError("Unsupported TIFF file [{}]: wrong format [{}]", path, sampleFormat);
	}

	TIFFClose(tiff);
}


//
//	OtHeightMap::save
//

void OtHeightMap::save(const std::string& path) {
	if (!heightmap) {
		OtLogError("Can't save invalid heightmap");
	}

	// open file for write
	auto options = TIFFOpenOptionsAlloc();
	TIFFOpenOptionsSetErrorHandlerExtR(options, errorHandler, this);
	auto tiff = TIFFOpenExt(path.c_str(), "w", options);
	TIFFOpenOptionsFree(options);

	if (!tiff) {
		OtLogError("Can't save to heightmap file [{}]", path);
	}

	// basic image dimensions
	TIFFSetField(tiff, TIFFTAG_IMAGEWIDTH, width);
	TIFFSetField(tiff, TIFFTAG_IMAGELENGTH, height);

	// Heightmap configuration: 1 channel, 32-bit
	TIFFSetField(tiff, TIFFTAG_SAMPLESPERPIXEL, 1);
	TIFFSetField(tiff, TIFFTAG_BITSPERSAMPLE, 32);
	TIFFSetField(tiff, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
	TIFFSetField(tiff, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
	TIFFSetField(tiff, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);

	// explicitly tell libtiff we are using unsigned integers
	TIFFSetField(tiff, TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_IEEEFP);

	// write data row by row (scanline)
	auto p = heightmap.get();
	auto rows = static_cast<uint32_t>(height);

	for (uint32_t row = 0; row < rows; row++) {
		if (TIFFWriteScanline(tiff, p, row) != 1) {
			OtLogError("Error while writing to TIFF file [{}]: {}", path, errorMessage);
		}
	}

	TIFFClose(tiff);
}


//
//	OtHeightMap::setHeight
//

void OtHeightMap::setHeight(int x, int y, float value) const {
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

float OtHeightMap::getHeight(int x, int y) const {
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

float OtHeightMap::getHeight(float x, float y) const {
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

glm::vec3 OtHeightMap::getNormal(float x, float y) const {
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

void OtHeightMap::getMinMaxHeights(float& minHeight, float& maxHeight) const {
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
