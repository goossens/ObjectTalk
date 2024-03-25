//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

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
	void load(const std::string& path, const std::string& format);

	// get image information
	int getWidth();
	int getHeight();

	// get pixel values
	OtObject getPixelRgba(int x, int y);
	float getPixelGray(int x, int y);

	OtObject sampleValueRgba(float x, float y);
	float sampleValueGray(float x, float y);

	// get type definition
	static OtType getMeta();

private:
	// the image
	OtImage image;
};
