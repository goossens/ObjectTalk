//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <memory>
#include <string>

#include "glm/glm.hpp"
#include "SDL3/SDL.h"


//
//	OtImage
//

class OtImage {
public:
	// image formats
	enum class Format {
		none = SDL_PIXELFORMAT_UNKNOWN,
		r8 = SDL_PIXELFORMAT_INDEX8,
		rgba8 = SDL_PIXELFORMAT_RGBA32,
		rgba32 = SDL_PIXELFORMAT_RGBA128_FLOAT
	};

	// constructors
	OtImage() = default;
	OtImage(const std::string& path, bool powerof2=false, bool square=false);
	OtImage(int width, int height, Format format, void* pixels);

	// clear the resources
	void clear();

	// create/update an image
	void update(int width, int height, Format format);

	// load image as RGBA
	void load(const std::string& address, bool powerof2=false, bool square=false);
	void load(void* data, size_t size);
	void load(int width, int height, Format format, void* pixels);

	// save the image to disk
	void saveToPNG(const std::string& path);

	// see if image is valid
	inline bool isValid() { return surface != nullptr; }

	// get information about image
	inline Format getFormat() { return static_cast<Format>(surface->format); }
	inline int getWidth() { return surface->w; }
	inline int getHeight() { return surface->h; }
	inline void* getPixels() { return surface->pixels; }
	inline int getPitch() { return surface->pitch; }

	inline int getBpp() {
		switch (getFormat()) {
			case Format::none: return 0;
			case Format::r8: return 1;
			case Format::rgba8: return 4;
			case Format::rgba32: return 16;
		}

		return 0;
	}

	// get pixel values
	glm::vec4 getPixelRgba(int x, int y);
	inline float getPixelGray(int x, int y) { return getPixelRgba(x, y).r; }

	glm::vec4 sampleValueRgba(float u, float v);
	float sampleValueGray(float u, float v);

	// version management
	inline void setVersion(int v) { version = v; }
	inline int getVersion() { return version; }
	inline void incrementVersion() { version++; }

	// see if images are identical
	inline bool operator==(OtImage& rhs) {
		return surface == rhs.surface && version == rhs.version;
	}

	inline bool operator!=(OtImage& rhs) {
		return !operator==(rhs);
	}

private:
	// the actual image
	friend class OtTexture;
	std::shared_ptr<SDL_Surface> surface;
	int version = 0;

	// memory manage SDL resource
	void assign(SDL_Surface* newSurface);

	// local utility functions
	void normalize();

	// load image from stream
	void load(SDL_IOStream *src);
};
