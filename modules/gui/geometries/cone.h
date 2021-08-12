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
//	OtConeClass
//

class OtConeClass;
typedef std::shared_ptr<OtConeClass> OtCone;

class OtConeClass : public OtGeometryClass {
public:
	// constructor
	OtConeClass();

	// initialize geometry
	OtObject init(size_t count, OtObject* parameters);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtCone create();

private:
	// create vertices
	void createTriangles();

	// geometry
	double radius = 1.0;
	double height = 1.0;
	int numSegments = 20;
	float thetaStart = 0.0;
	float thetaLength = std::numbers::pi * 2.0;
};
