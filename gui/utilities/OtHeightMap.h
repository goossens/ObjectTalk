//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "bimg/bimg.h"
#include "glm/glm.hpp"

#include "OtController.h"
#include "OtGui.h"


//
//	OtHeightMapClass
//

class OtHeightMapClass;
typedef std::shared_ptr<OtHeightMapClass> OtHeightMap;

class OtHeightMapClass : public OtGuiClass {
public:
	// destructor
	~OtHeightMapClass();

	// initialize
	OtObject init(size_t count, OtObject* parameters);

	// load heightmap from image
	OtObject loadMap(const std::string& file);

	// set the heightmap scale factor
	OtObject setScale(float scale);

	// set the heightmap offset (after scaling)
	OtObject setOffset(float offset);

	// get height (0.0 to 1.0) at specified location (in relative coordinates, 0.0 to 1.0)
	float getHeight(float x, float y);

	// get normal (as normalized vector) at specified location (in relative coordinates, 0.0 to 1.0)
	glm::vec3 getNormal(float x, float y);

	// GUI to change properties
	void renderGUI();

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtHeightMap create();

private:
	// convert 8-bit unsigned image to heightmap
	void convert8bit(uint8_t* data);

	// convert 8-bit unsigned image to heightmap
	void convert16bit(uint16_t* data);

	// convert other image types
	void convertImage(bimg::ImageContainer* image);

	// get height (0.0 to 1.0) at specified location (in absolute coordinates)
	float getHeightAbs(int x, int y);

	// heightmap properties
	int width;
	int height;

	float scale = 1.0;
	float offset = 0.0;

	float* heightmap = nullptr;
};


//
//	Controller widget
//

OT_CONTROLLER(HeightMap)
