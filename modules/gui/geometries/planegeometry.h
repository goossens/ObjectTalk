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
//	OtPlaneGeometryClass
//

class OtPlaneGeometryClass;
typedef std::shared_ptr<OtPlaneGeometryClass> OtPlaneGeometry;

class OtPlaneGeometryClass : public OtGeometryClass {
public:
	// initialize geometry
	OtObject init(size_t count, OtObject* parameters);

	// update attributes
	OtObject setWidth(float width);
	OtObject setHeight(float height);
	OtObject setWidthSegments(int widthSegments);
	OtObject setHeightSegments(int heightSegments);

	// get information
	float getWidth() { return width; }
	float getHeight() { return height; }
	int getWidthSegments() { return widthSegments; }
	int getHeightSegments() { return heightSegments; }

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtPlaneGeometry create();

private:
	// generate geometry
	void fillGeometry();

	// properties
	float width = 1.0;
	float height = 1.0;
	int widthSegments = 1;
	int heightSegments = 1;
};
