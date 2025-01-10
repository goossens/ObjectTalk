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
	static constexpr int r8Image = bimg::TextureFormat::R8;
	static constexpr int r16Image = bimg::TextureFormat::R16;
	static constexpr int rFloat32Image = bimg::TextureFormat::R32F;
	static constexpr int rgFloat16Image = bimg::TextureFormat::RG16F;
	static constexpr int rgFloat32Image = bimg::TextureFormat::RG32F;
	static constexpr int rgba8Image = bimg::TextureFormat::RGBA8;
	static constexpr int rgba16Image = bimg::TextureFormat::RGBA16;
	static constexpr int rgbaFloat16Image = bimg::TextureFormat::RGBA16F;
	static constexpr int rgbaFloat32Image = bimg::TextureFormat::RGBA32F;

	// constructors/destructor
	OtImage() = default;
	OtImage(const std::string& path, bool powerof2=false, bool square=false);

	// clear the resources
	void clear();

	// create/update an image
	void update(int width, int height, int format);

	// load the image from disk
	void load(const std::string& path, bool powerof2=false, bool square=false);
	void loadAsGrayscale(const std::string& path, bool powerof2=false, bool square=false);
	void loadAsRGBA(const std::string& path, bool powerof2=false, bool square=false);

	// load the image from a file in memory
	void loadFromFileInMemory(void* data, uint32_t size);

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
};
