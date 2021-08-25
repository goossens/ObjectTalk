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
//	OtPlaneClass
//

class OtPlaneClass;
typedef std::shared_ptr<OtPlaneClass> OtPlane;

class OtPlaneClass : public OtGeometryClass {
public:
	// initialize geometry
	OtObject init(size_t count, OtObject* parameters);

	// update attributes
	OtObject setSize(double width, double height);
	OtObject setSegments(int widthSegments, int heightSegments);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtPlane create();
	static OtPlane create(double width, double height);

private:
	// generate geometry
	void fillBuffers();

	// attributes
	double width = 1.0;
	double height = 1.0;
	int widthSegments = 1;
	int heightSegments = 1;
};
