//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "ot/numbers.h"

#include "geometry.h"


//
//	OtCylinderClass
//

class OtCylinderClass;
typedef std::shared_ptr<OtCylinderClass> OtCylinder;

class OtCylinderClass : public OtGeometryClass {
public:
	// constructor
	OtCylinderClass();

	// initialize geometry
	OtObject init(size_t count, OtObject* parameters);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtCylinder create();

private:
	// create vertices
	void createTriangles();

	// resolution
	double radius = 1.0;
	double height = 1.0;
	int numSegments = 8;
	float start = 0.0;
	float end = std::numbers::pi * 2.0;
};
