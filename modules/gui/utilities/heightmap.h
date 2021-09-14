//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "glm/glm.hpp"

#include "gui.h"


//
//	OtHeightMapClass
//

class OtHeightMapClass;
typedef std::shared_ptr<OtHeightMapClass> OtHeightMap;

class OtHeightMapClass : public OtGuiClass {
public:
	// destructor
	~OtHeightMapClass();

	// initialize heightmap
	void init(const std::string& file, double scale);

	// get height (0.0 to 1.0) at specified location (in relative coordinates, 0.0 to 1.0)
	double getHeight(double x, double y);

	// get normal (as normalized vector) at specified location (in relative coordinates, 0.0 to 1.0)
	glm::vec3 getNormal(double x, double y);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtHeightMap create();

private:
	// get height (0.0 to 1.0) at specified location (in absolute coordinates)
	double getHeightAbs(int x, int y);

	// height data
	int width;
	int height;
	double* heightmap = nullptr;
	double scale = 1.0;
};
