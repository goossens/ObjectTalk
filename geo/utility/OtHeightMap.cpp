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
#include "OtHash.h"
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
	if (!data || width != w || height != h) {
		data = std::make_shared<float[]>(w * h);
		width = w;
		height = h;
		incrementVersion();
	}

	// clear values if required
	if (clear) {
		std::fill(data.get(), data.get() + w * h, 0.0f);
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
		auto p = data.get();

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
	if (!data) {
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
	auto p = data.get();
	auto rows = static_cast<uint32_t>(height);

	for (uint32_t row = 0; row < rows; row++) {
		if (TIFFWriteScanline(tiff, p, row) != 1) {
			OtLogError("Error while writing to TIFF file [{}]: {}", path, errorMessage);
		}

		p += width;
	}

	TIFFClose(tiff);
}


//
//	OtHeightMap::loadFromImage
//

void OtHeightMap::loadFromImage(const OtImage& image, float minValue, float maxValue) {
	if (image.isValid()) {
		auto range = maxValue - minValue;
		auto w = image.getWidth();
		auto h = image.getHeight();
		update(w, h);

		for (auto y = 0; y < h; y++) {
			for (auto x = 0; x < w; x++) {
				data[y * width + x] = image.getPixelGray(x, y) * range + minValue;
			}
		}

		incrementVersion();

	} else {
		clear();
	}
}


//
//	OtHeightMap::saveToImage
//

void OtHeightMap::saveToImage(OtImage& image) {
	if (isValid()) {
		auto minValue = getMinElevation();
		auto maxValue = getMaxElevation();
		auto range = maxValue - minValue;
		image.update(width, height, OtImage::Format::rgba32);

		for (auto y = 0; y < height; y++) {
			for (auto x = 0; x < width; x++) {
				image.setPixelGray(x, y, (getElevation(x, y) - minValue) / range);
			}
		}

		image.incrementVersion();

	} else {
		image.clear();
	}
}


//
//	OtHeightMap::saveToTexture
//

void OtHeightMap::saveToTexture(OtTexture& texture) {
	if (isValid()) {
		texture.update(width, height, OtTexture::Format::r32, OtTexture::Usage::rwDefault);
		texture.update(0, 0, width, height, data.get());

	} else {
		clear();
	}
}


//
//	OtHeightMap::clone
//

OtHeightMap OtHeightMap::clone() {
	OtHeightMap hm;

	if (isValid()) {
		hm.update(width, height);
		std::copy(data.get(), data.get() + width * height, hm.data.get());
	}

	hm.setVersion(getVersion());
	return hm;
}


//
//	OtHeightMap::clear
//

void OtHeightMap::clear() {
	data = nullptr;
	width = 0;
	height = 0;
	incrementVersion();
}


//
//	OtHeightMap::setElevation
//

void OtHeightMap::setElevation(int x, int y, float value) const {
	// sanity checks
	OtAssert(data != nullptr);
	OtAssert(x >= 0 && x < width);
	OtAssert(y >= 0 && y < height);

	// set height
	data[y * width + x] = value;
}


//
//	OtHeightMap::adjustElevation
//

void OtHeightMap::adjustElevation(int x, int y, float value) const {
	// sanity checks
	OtAssert(data != nullptr);
	OtAssert(x >= 0 && x < width);
	OtAssert(y >= 0 && y < height);

	// adjust height
	data[y * width + x] += value;
}


//
//	OtHeightMap::getNormal
//

glm::vec3 OtHeightMap::getNormal(int x, int y) const {
	// determine surface normal using Sobel filter
	static constexpr auto w10 = glm::vec3(0.1f);
	static constexpr auto w15 = glm::vec3(0.15f);
	static constexpr auto scale = 60.0f;
	static constexpr auto sqrt2 = 1.41421356f;

	auto tl = getElevation(x - 1, y - 1);
	auto tc = getElevation(x, y - 1);
	auto tr = getElevation(x + 1, y - 1);
	auto ml = getElevation(x - 1, y);
	auto mc = getElevation(x, y);
	auto mr = getElevation(x + 1, y);
	auto bl = getElevation(x - 1, y + 1);
	auto bc = getElevation(x, y + 1);
	auto br = getElevation(x + 1, y + 1);

	glm::vec3 n;
	n += w15 * glm::normalize(glm::vec3(scale * (mc - mr), 1.0f, 0.0f));
	n += w15 * glm::normalize(glm::vec3(scale * (ml - mc), 1.0f, 0.0f));
	n += w15 * glm::normalize(glm::vec3(0.0f, 1.0f, scale * (mc - bc)));
	n += w15 * glm::normalize(glm::vec3(0.0f, 1.0f, scale * (tc - mc)));

	n += w10 * glm::normalize(glm::vec3(scale * (mc - tl) / sqrt2, sqrt2, scale * (mc - tl) / sqrt2));
	n += w10 * glm::normalize(glm::vec3(scale * (mc - tr) / sqrt2, sqrt2, scale * (mc - tr) / sqrt2));
	n += w10 * glm::normalize(glm::vec3(scale * (mc - bl) / sqrt2, sqrt2, scale * (mc - bl) / sqrt2));
	n += w10 * glm::normalize(glm::vec3(scale * (mc - br) / sqrt2, sqrt2, scale * (mc - br) / sqrt2));

	return n;
}


//
//	OtHeightMap::sampleElevation
//

float OtHeightMap::sampleElevation(float x, float y) const {
	x *= width - 1;
	y *= height - 1;

	int x1 = static_cast<int>(std::floor(x));
	int y1 = static_cast<int>(std::floor(y));
	int x2 = x1 + 1;
	int y2 = y1 + 1;

	auto h11 = getElevation(x1, y1);
	auto h21 = getElevation(x2, y1);
	auto h12 = getElevation(x1, y2);
	auto h22 = getElevation(x2, y2);

	auto hx1 = std::lerp(h11, h21, x - x1);
	auto hx2 = std::lerp(h12, h22, x - x1);
	return std::lerp(hx1, hx2, y - y1);
}


//
//	OtHeightMap::sampleNormal
//

glm::vec3 OtHeightMap::sampleNormal(float x, float y) const {
	// determine surface normal using Sobel filter
	static constexpr auto w10 = glm::vec3(0.1f);
	static constexpr auto w15 = glm::vec3(0.15f);
	static constexpr auto scale = 60.0f;
	static constexpr auto sqrt2 = 1.41421356f;

	auto tl = sampleElevation(x - 1.0f, y - 1.0f);
	auto tc = sampleElevation(x, y - 1.0f);
	auto tr = sampleElevation(x + 1.0f, y - 1.0f);
	auto ml = sampleElevation(x - 1.0f, y);
	auto mc = sampleElevation(x, y);
	auto mr = sampleElevation(x + 1.0f, y);
	auto bl = sampleElevation(x - 1.0f, y + 1.0f);
	auto bc = sampleElevation(x, y + 1.0f);
	auto br = sampleElevation(x + 1.0f, y + 1.0f);

	glm::vec3 n;
	n += w15 * glm::normalize(glm::vec3(scale * (mc - mr), 1.0f, 0.0f));
	n += w15 * glm::normalize(glm::vec3(scale * (ml - mc), 1.0f, 0.0f));
	n += w15 * glm::normalize(glm::vec3(0.0f, 1.0f, scale * (mc - bc)));
	n += w15 * glm::normalize(glm::vec3(0.0f, 1.0f, scale * (tc - mc)));

	n += w10 * glm::normalize(glm::vec3(scale * (mc - tl) / sqrt2, sqrt2, scale * (mc - tl) / sqrt2));
	n += w10 * glm::normalize(glm::vec3(scale * (mc - tr) / sqrt2, sqrt2, scale * (mc - tr) / sqrt2));
	n += w10 * glm::normalize(glm::vec3(scale * (mc - bl) / sqrt2, sqrt2, scale * (mc - bl) / sqrt2));
	n += w10 * glm::normalize(glm::vec3(scale * (mc - br) / sqrt2, sqrt2, scale * (mc - br) / sqrt2));

	return n;
}


//
//	OtHeightMap::getMinElevation
//

float OtHeightMap::getMinElevation() const {
	auto minElevation = std::numeric_limits<float>::max();

	for (auto y = 0; y < height; y++) {
		for (auto x = 0; x < width; x++) {
			auto h = getElevation(x, y);
			minElevation = std::min(minElevation, h);
		}
	}

	return minElevation;
}


//
//	OtHeightMap::getMaxElevation
//

float OtHeightMap::getMaxElevation() const {
	auto maxElevation = std::numeric_limits<float>::lowest();

	for (auto y = 0; y < height; y++) {
		for (auto x = 0; x < width; x++) {
			auto h = getElevation(x, y);
			maxElevation = std::max(maxElevation, h);
		}
	}

	return maxElevation;
}


//
//	OtHeightMap::erode
//

void OtHeightMap::erode(int run, int drops) {
	for (auto i = 0; i < drops; i++) {
		// pick random location for a drop of water
		Particle drop(glm::vec2(
			OtHash::toFloat(
				static_cast<uint32_t>(seed),
				static_cast<uint32_t>(run),
				static_cast<uint32_t>(i + 17)) * (width - 1),
			OtHash::toFloat(
				static_cast<uint32_t>(seed),
				static_cast<uint32_t>(run),
				static_cast<uint32_t>(i + 67)) * (height - 1)));

		// run until water drop is "dry" or the flow is now out-of-bounds
		bool outOfBounds = false;

		while (drop.volume > minimumVolume && !outOfBounds) {
			// get surface normal at drop
			glm::ivec2 ipos = drop.pos;
			glm::vec3 n = getNormal(ipos.x, ipos.y);

			// accelerate particle using newtonian mechanics using the surface normal
			drop.speed += dt * glm::vec2(n.x, n.z)  / (drop.volume * density);
			drop.pos += dt * drop.speed;
			drop.speed *= 1.0f - dt * friction;

			// ensure drop is inbounds
			if (glm::all(glm::greaterThanEqual(drop.pos, glm::vec2(0.0f))) && glm::all(glm::lessThan(drop.pos, glm::vec2(width, height)))) {
				// compute sediment capacity difference
				glm::ivec2 newIpos = drop.pos;
				auto elevationChange = data[ipos.y * width + ipos.x] - data[newIpos.y * width + newIpos.x];
				float maxSediment = std::max(drop.volume * glm::length(drop.speed) * elevationChange, 0.0f);
				float sdiff = maxSediment - drop.sediment;

				// adjust heightmap and droplet
				drop.sediment += dt * depositionRate * sdiff;
				data[newIpos.y * width + newIpos.x] += dt * drop.volume * depositionRate * sdiff;

				// evaporate the droplet
				drop.volume *= (1.0f - dt * evaporationRate);

			} else {
				outOfBounds = true;
			}
		}
	}

	incrementVersion();
}


//
//	OtHeightMap::islandize
//

template<typename F>
void applyIsland(float* output, float* input, int w, int h, float minElevation, F function) {
	auto w2 = w >> 1;
	auto h2 = h >> 1;
	auto scaleX = 2.0f / static_cast<float>(w);
	auto scaleY = 2.0f / static_cast<float>(h);

	for (auto y = 0; y < h; y++) {
		for (auto x = 0; x < w; x++) {
			auto height = *(input + y * w + x) - minElevation;
			auto distance = function(static_cast<float>(x - w2) * scaleX, static_cast<float>(y - h2) * scaleY);
			*output++ = height * (1.0f - distance) + minElevation;
		}
	}
}

void OtHeightMap::islandize(DistanceFunction distanceFunction) {
	auto newData = std::make_shared<float[]>(width * height);

	switch (distanceFunction) {
		case DistanceFunction::squareBump:
			applyIsland(
				newData.get(), data.get(),
				width, height,
				getMinElevation(),
				[](float x, float y) { return 1.0f - (1.0f - x * x) * (1.0f - y * y); });

			break;

		case DistanceFunction::euclidean:
			applyIsland(
				newData.get(), data.get(),
				width, height,
				getMinElevation(),
				[](float x, float y) { return std::sqrt(x * x + y * y) / 1.414213562373095f; });

			break;

		case DistanceFunction::euclidean2:
			applyIsland(
				newData.get(), data.get(),
				width, height,
				getMinElevation(),
				[](float x, float y) { return (x * x + y * y) / 2.0f; });

			break;

		case DistanceFunction::hyperboloid: {
			static constexpr float C = 0.2f;

			applyIsland(
				newData.get(), data.get(),
				width, height,
				getMinElevation(),
				[](float x, float y) { return (std::sqrt(x * x + y * y + C * C) - C) / (std::sqrt(1.0f + C * C) - C); });

			break;
		}

		case DistanceFunction::diagonal:
			applyIsland(
				newData.get(), data.get(),
				width, height,
				getMinElevation(),
				[](float x, float y) { return std::max(std::abs(x), std::abs(y)); });

			break;
	}

	data = newData;
	incrementVersion();
}
