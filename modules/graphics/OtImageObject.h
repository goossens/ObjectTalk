//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <filesystem>
#include <string>

#include "OtObject.h"

#include "OtImage.h"


//
//	OtImageObjectClass
//

class OtImageObjectClass;
using OtImageObject = OtObjectPointer<OtImageObjectClass>;

class OtImageObjectClass : public OtObjectClass {
public:
	// initialize
	void init(size_t count, OtObject* parameters);

	// load image
	void load(const std::filesystem::path& path, const std::string& format);

	// get image information
	size_t getWidth();
	size_t getHeight();

	// get pixel values
	OtObject getPixelRgba(size_t x, size_t y);
	float getPixelGray(size_t x, size_t y);
	float sampleValue(float x, float y);

	// get type definition
	static OtType getMeta();

private:
	// the image
	OtImage image;
};
