//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtException.h"
#include "OtFunction.h"

#include "OtImageObject.h"
#include "OtVec4.h"


//
//	OtImageObjectClass::init
//

void OtImageObjectClass::init(size_t count, OtObject* parameters) {
	if (count == 2) {
		load(std::string(
			parameters[0]->operator std::string()),
			parameters[1]->operator std::string());

	} else if (count !=0) {
		OtError("[Image] constructor expects 0 or 2 arguments (not {})", count);
	}
}


//
//	OtImageObjectClass::load
//

void OtImageObjectClass::load(const std::string& path, const std::string& format) {
	// load image based on requested format
	if (format == "grayscale") {
		image.loadAsGrayscale(path);

	} else if (format == "rgba") {
		image.loadAsRGBA(path);

	} else {
		OtError("Invalid image format (expected [grayscale] or [rgba])");
	}
}


//
//	OtImageObjectClass::getWidth
//

size_t OtImageObjectClass::getWidth() {
	// sanity check
	if (!image.isValid()) {
		OtError("Image not yet loaded");
	}

	return image.getWidth();
}


//
//	OtImageObjectClass::getHeight
//

size_t OtImageObjectClass::getHeight() {
	// sanity check
	if (!image.isValid()) {
		OtError("Image not yet loaded");
	}

	return image.getHeight();
}


//
//	OtImageObjectClass::getPixelRgba
//

OtObject OtImageObjectClass::getPixelRgba(size_t x, size_t y) {
	// sanity check
	if (!image.isValid()) {
		OtError("Image not yet loaded");
	}

	if (x >= image.getWidth()) {
		OtError("Image x coordinate [{}] out of range [max {}]", x, image.getWidth());

	}

	if (y >= image.getHeight()) {
		OtError("Image y coordinate [{}] out of range [max {}]", x, image.getHeight());

	}

	return OtVec4::create(image.getPixelRgba(x, y));
}


//
//	eObjectClass::getPixelGray
//

float OtImageObjectClass::getPixelGray(size_t x, size_t y) {
		// sanity check
	if (!image.isValid()) {
		OtError("Image not yet loaded");
	}

	if (x >= image.getWidth()) {
		OtError("Image x coordinate [{}] out of range [max {}]", x, image.getWidth());

	}

	if (y >= image.getHeight()) {
		OtError("Image y coordinate [{}] out of range [max {}]", y, image.getHeight());

	}

	return image.getPixelGray(x, y);
}


//
//	OtImageObjectClass::sampleValue
//

float OtImageObjectClass::sampleValue(float x, float y) {
		// sanity check
	if (!image.isValid()) {
		OtError("Image not yet loaded");
	}

	if (x < 0.0f || x > 1.0f) {
		OtError("Image x coordinate [{}] out of range [0.0 - 1.0]", x);

	}

	if (y < 0.0f || y > 1.0f) {
		OtError("Image y coordinate [{}] out of range [0.0 - 1.0]", y);

	}

	return image.sampleValue(x, y);
}


//
//	OtImageObjectClass::getMeta
//

OtType OtImageObjectClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtType::create<OtImageObjectClass>("Image", OtObjectClass::getMeta());
		type->set("__init__", OtFunction::create(&OtImageObjectClass::init));
		type->set("load", OtFunction::create(&OtImageObjectClass::load));

		type->set("getWidth", OtFunction::create(&OtImageObjectClass::getWidth));
		type->set("getHeight", OtFunction::create(&OtImageObjectClass::getHeight));

		type->set("getPixelRgba", OtFunction::create(&OtImageObjectClass::getPixelRgba));
		type->set("getPixelGray", OtFunction::create(&OtImageObjectClass::getPixelGray));
		type->set("sampleValue", OtFunction::create(&OtImageObjectClass::sampleValue));
	}

	return type;
}
