//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
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

#include "bimg/bimg.h"


//
//	OtImage
//

class OtImage {
public:
	// image types
	static constexpr int rgba8Image = bimg::TextureFormat::RGBA8;
	static constexpr int rgbaFloat32Image = bimg::TextureFormat::RGBA32F;

	// constructors
	OtImage() = default;
	OtImage(const std::string& path, bool powerof2=false, bool square=false);

	// clear the resources
	void clear();

	// create/update an image
	void update(int width, int height, int format);

	// load image
	void load(const std::string& address, bool powerof2=false, bool square=false);
	void loadAsGrayscale(const std::string& address, bool powerof2=false, bool square=false);
	void loadAsRGBA(const std::string& address, bool powerof2=false, bool square=false);
	void load(int width, int height, int format, void* pixels);
	void load(void* data, size_t size);

	// save the image to disk
	void saveToPNG(const std::string& path);
	void saveToDDS(const std::string& path);

	// see if image is valid
	inline bool isValid() { return image != nullptr; }

	// get information about image
	bimg::ImageContainer* getContainer();
	inline int getWidth() { return (int) image->m_width; }
	inline int getHeight() { return (int) image->m_height; }
	inline void* getPixels() { return image->m_data; }
	inline int getBitsPerPixel() { return (int) bimg::getBitsPerPixel(image->m_format); }

	// get pixel values
	glm::vec4 getPixelRgba(int x, int y);
	float getPixelGray(int x, int y);

	glm::vec4 sampleValueRgba(float u, float v);
	float sampleValueGray(float u, float v);

	// version management
	inline void setVersion(int v) { version = v; }
	inline int getVersion() { return version; }
	inline void incrementVersion() { version++; }

	// see if images are identical
	inline bool operator==(OtImage& rhs) {
		return image == rhs.image && version == rhs.version;
	}

	inline bool operator!=(OtImage& rhs) {
		return !operator==(rhs);
	}

private:
	// the actual image
	std::shared_ptr<bimg::ImageContainer> image;
	int version = 0;

	// assign a new image container to the shared pointer
	void assignImageContainer(bimg::ImageContainer* container);

	void loadFromFile(const std::string& path);
	void loadFromUrl(const std::string& url);
};
