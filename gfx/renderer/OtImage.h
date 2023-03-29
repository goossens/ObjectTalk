//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>
#include <filesystem>

#include "bimg/bimg.h"


//
//	OtImage
//

class OtImage {
public:
	// constructors/destructor
	OtImage() = default;
	OtImage(const std::filesystem::path& path, bool powerof2=false, bool square=false);
	OtImage(const OtImage&) = delete; // no copy constructor
	OtImage& operator=(const OtImage&) = delete; // no copy assignment
	OtImage(OtImage&&) = default;
	OtImage& operator=(OtImage&&) = default;
	~OtImage();

	// clear the resources
	void clear();

	// load the image from disk
	void load(const std::filesystem::path& path, bool powerof2=false, bool square=false);
	void loadAsGrayscale(const std::filesystem::path& path, bool powerof2=false, bool square=false);
	void loadAsRGBA(const std::filesystem::path& path, bool powerof2=false, bool square=false);

	// load the image from a file in memory
	void loadFromFileInMemory(void* data, uint32_t size);

	// see if image is valid
	bool isValid() { return image != nullptr; }

	// get information about image
	bimg::ImageContainer* getContainer();
	int getWidth() { return (int) image->m_width; }
	int getHeight() { return (int) image->m_height; }
	void* getPixels() { return image->m_data; }
	int getBitsPerPixel() { return (int) bimg::getBitsPerPixel(image->m_format); }

private:
	// the actual image
	bimg::ImageContainer* image = nullptr;
};