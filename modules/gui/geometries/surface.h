//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "geometry.h"


//
//	OtSurfaceClass
//

class OtSurfaceClass;
typedef std::shared_ptr<OtSurfaceClass> OtSurface;

class OtSurfaceClass : public OtGeometryClass {
public:
	// initialize geometry
	OtObject init(size_t count, OtObject* parameters);

	// update attributes
	OtObject setWidth(double width);
	OtObject setHeight(double height);
	OtObject setWidthSegments(int widthSegments);
	OtObject setHeightSegments(int heightSegments);

	// get information
	double getWidth() { return width; }
	double getHeight() { return height; }
	int getWidthSegments() { return widthSegments; }
	int getHeightSegments() { return heightSegments; }

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtSurface create();

private:
	// generate geometry
	void fillGeometry();

	// attributes
	double width = 1.0;
	double height = 1.0;
	int widthSegments = 1;
	int heightSegments = 1;
};
