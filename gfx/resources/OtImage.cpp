//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>
#include <cstring>
#include <fstream>

#include "stb_image.h"
#include "SDL3/SDL_iostream.h"

#include "OtAssert.h"
#include "OtException.h"
#include "OtLog.h"
#include "OtNumbers.h"
#include "OtText.h"

#include "OtImage.h"
#include "OtFrameworkAtExit.h"
#include "OtUrl.h"


//
//	OtImage::OtImage
//

OtImage::OtImage(const std::string& path, bool powerof2, bool square) {
	load(path, powerof2, square);
}

OtImage::OtImage(int width, int height, Format format, void* pixels) {
	load(width, height, format, pixels);
}


//
//	OtImage::clear
//

void OtImage::clear() {
	if (surface) {
		surface = nullptr;
		incrementVersion();
	}
}


//
//	isPowerOfTwo
//

static bool isPowerOfTwo(int n) {
	// handle the special case of 0, which is not a power of two
	if (n <= 0) {
		return false;

	} else {
		// check if n has only one bit set using the bitwise AND trick
		return (n & (n - 1)) == 0;
	}
}


//
//	OtImage::update
//

void OtImage::update(int width, int height, Format format) {
	// update surface (if required)
	auto fmt = static_cast<SDL_PixelFormat>(format);

	if (!surface || surface->w != width || surface->h != height || surface->format != fmt) {
		auto sdlSurface = SDL_CreateSurface(width, height, fmt);

		if (!sdlSurface) {
			OtLogFatal("Error in SDL_CreateSurface: {}", SDL_GetError());
		}

		assign(sdlSurface);
	}
}


//
//	OtImage::load
//

void OtImage::load(const std::string& address, bool powerof2, bool square) {
	// do we have a URL?
	if (OtText::startsWith(address, "http:") || OtText::startsWith(address, "https:")) {
		OtUrl url(address);
		url.download();
		load(url.getDownloadedData(), url.getDownloadedSize());

	} else {
		// load image from file
		auto io = SDL_IOFromFile(address.c_str(), "r");

		if (!io) {
			OtLogFatal("Error in SDL_IOFromMem: {}", SDL_GetError());
		}

		load(io);
		SDL_CloseIO(io);
	}

	// validate sides are power of 2 (if required)
	if (powerof2 && !(isPowerOfTwo(surface->w))) {
		clear();
		OtLogError("Image width {} is not a power of 2", surface->w);
	}

	if (powerof2 && !(isPowerOfTwo(surface->h))) {
		clear();
		OtLogError("Image height {} is not a power of 2", surface->h);
	}

	// validate squareness (if required)
	if (square && surface->w != surface->h) {
		clear();
		OtLogError("Image must be square not {} by {}", surface->w, surface->h);
	}
}


//
//	OtImage::load
//

void OtImage::load(void* data, size_t size) {
	// create the image
	auto io = SDL_IOFromMem(data, size);

	if (!io) {
		OtLogFatal("Error in SDL_IOFromMem: {}", SDL_GetError());
	}

	load(io);
	SDL_CloseIO(io);
}


//
//	OtImage::load
//

void OtImage::load(SDL_IOStream* src) {
	// setup stream
	stbi_io_callbacks callbacks;

	callbacks.read = [](void* user, char* data, int size) {
		return static_cast<int>(SDL_ReadIO((SDL_IOStream*) user, data, size));
	};

	callbacks.skip = [](void* user, int n) {
		SDL_SeekIO((SDL_IOStream*) user, n, SDL_IO_SEEK_CUR);
	};

	callbacks.eof = [](void* user) {
		return static_cast<int>(SDL_GetIOStatus((SDL_IOStream*) user) == SDL_IO_STATUS_EOF);
	};

	// read image
	int w, h, n;
	auto pixels = stbi_load_from_callbacks(&callbacks, src, &w, &h, &n, 4);

	if (pixels) {
		load(w, h, Format::rgba8, pixels);
		stbi_image_free(pixels);

	} else {
		SDL_SetError("%s", stbi_failure_reason());
	}
}


//
//	OtImage::load
//

void OtImage::load(int width, int height, Format format, void* pixels) {
	// create new surface
	auto sdlSurface = SDL_CreateSurface(width, height, static_cast<SDL_PixelFormat>(format));

	if (!sdlSurface) {
		OtLogFatal("Error in SDL_CreateSurface: {}", SDL_GetError());
	}

	if (format == Format::r8) {
		auto palette = SDL_CreateSurfacePalette(sdlSurface);

		if (!palette) {
			 SDL_DestroySurface(sdlSurface);
			OtLogFatal("Error in SDL_CreateSurfacePalette: {}", SDL_GetError());
		}

		for (int i = 0; i < palette->ncolors; i++) {
			palette->colors[i].r = static_cast<Uint8>(i);
			palette->colors[i].g = static_cast<Uint8>(i);
			palette->colors[i].b = static_cast<Uint8>(i);
		}
	}

	if (SDL_MUSTLOCK(sdlSurface)) {
		SDL_LockSurface(sdlSurface);
		std::memcpy(sdlSurface->pixels, pixels, sdlSurface->h * sdlSurface->pitch);
		SDL_UnlockSurface(sdlSurface);

	} else {
		std::memcpy(sdlSurface->pixels, pixels, sdlSurface->h * sdlSurface->pitch);
	}

	assign(sdlSurface);
	normalize();
}


//
//	OtImage::saveToPNG
//

void OtImage::saveToPNG(const std::string& path) {
	// sanity check
	OtAssert(isValid());

	// write image to file
	if (!SDL_SavePNG(surface.get(), path.c_str())){
		OtLogFatal("Error in SDL_SavePNG: {}", SDL_GetError());
	}
}


//
//	OtImage::getPixelRgba
//

glm::vec4 OtImage::getPixelRgba(int x, int y) {
	// sanity check
	OtAssert(isValid());

	x = std::clamp(x, 0, surface->w - 1);
	y = std::clamp(y, 0, surface->h - 1);

	if (surface->format == SDL_PIXELFORMAT_RGBA32) {
		auto p = static_cast<uint8_t*>(surface->pixels);
		p += surface->pitch * y + x * 4;

		return glm::vec4(
			static_cast<float>(p[0]) / 255.0f,
			static_cast<float>(p[1]) / 255.0f,
			static_cast<float>(p[2]) / 255.0f,
			static_cast<float>(p[3]) / 255.0f);

	} else if (surface->format == SDL_PIXELFORMAT_RGBA128_FLOAT) {
		auto p = static_cast<uint8_t*>(surface->pixels);
		p += surface->pitch * y + x * 16;
		auto f = (float*) p;
		return glm::vec4(f[0], f[1], f[2], f[3]);

	} else {
		OtLogFatal("Internal error: invalid pixel format");
		return glm::vec4();
	}
}


//
//	OtImage::sampleValueRgba
//

glm::vec4 OtImage::sampleValueRgba(float x, float y) {
	// sanity check
	OtAssert(isValid());

	x *= surface->w - 1;
	y *= surface->h - 1;

	int x1 = static_cast<int>(std::floor(x));
	int y1 = static_cast<int>(std::floor(y));
	int x2 = x1 + 1;
	int y2 = y1 + 1;

	auto h11 = getPixelRgba(x1, y1);
	auto h21 = getPixelRgba(x2, y1);
	auto h12 = getPixelRgba(x1, y2);
	auto h22 = getPixelRgba(x2, y2);

	auto hx1 = glm::mix(h11, h21, x - x1);
	auto hx2 = glm::mix(h12, h22, x - x1);
	return glm::mix(hx1, hx2, y - y1);
}


//
//	OtImage::sampleValueGray
//

float OtImage::sampleValueGray(float x, float y) {
	// sanity check
	OtAssert(isValid());

	x *= surface->w - 1;
	y *= surface->h - 1;

	int x1 = static_cast<int>(std::floor(x));
	int y1 = static_cast<int>(std::floor(y));
	int x2 = x1 + 1;
	int y2 = y1 + 1;

	auto h11 = getPixelGray(x1, y1);
	auto h21 = getPixelGray(x2, y1);
	auto h12 = getPixelGray(x1, y2);
	auto h22 = getPixelGray(x2, y2);

	auto hx1 = std::lerp(h11, h21, x - x1);
	auto hx2 = std::lerp(h12, h22, x - x1);
	return std::lerp(hx1, hx2, y - y1);
}


//
//	OtImage::assign
//

void OtImage::assign(SDL_Surface* newSurface) {
	surface = std::shared_ptr<SDL_Surface>(
		newSurface,
		[](SDL_Surface* oldSurface) {
			SDL_DestroySurface(oldSurface);
		});

	incrementVersion();
}


//
//	OtImage::normalize
//

void OtImage::normalize() {
	if (SDL_ISPIXELFORMAT_FLOAT(surface->format)) {
		if (surface->format != SDL_PIXELFORMAT_RGBA128_FLOAT) {
			auto sdlSurface = SDL_ConvertSurface(surface.get(), SDL_PIXELFORMAT_RGBA128_FLOAT);

			if (!sdlSurface) {
				OtLogFatal("Error in SDL_ConvertSurface: {}", SDL_GetError());
			}

			assign(sdlSurface);
		}

	} else {
		if (surface->format != SDL_PIXELFORMAT_INDEX8 && surface->format != SDL_PIXELFORMAT_RGBA32) {
			auto sdlSurface = SDL_ConvertSurface(surface.get(), SDL_PIXELFORMAT_RGBA32);

			if (!sdlSurface) {
				OtLogFatal("Error in SDL_ConvertSurface: {}", SDL_GetError());
			}

			assign(sdlSurface);
		}
	}
}
