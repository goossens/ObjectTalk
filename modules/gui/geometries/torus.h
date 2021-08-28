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
//	OtTorusClass
//

class OtTorusClass;
typedef std::shared_ptr<OtTorusClass> OtTorus;

class OtTorusClass : public OtGeometryClass {
public:
	// initialize geometry
	OtObject init(size_t count, OtObject* parameters);

	// update attributes
	OtObject setRadius(double radius);
	OtObject setTubeRadius(double tubeRadius);
	OtObject setRadialSegments(int segments);
	OtObject setTubularSegments(int segments);
	OtObject setRadialStart(double radialStart);
	OtObject setRadialLength(double radialLength);
	OtObject setTubularStart(double tubularStart);
	OtObject setTubularLength(double tubularLength);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtTorus create();

private:
	// generate geometry
	void fillBuffers();

	// attributes
	double radius = 1.0;
	double tubeRadius = 0.4;
	int radialSegments = 32;
	int tubularSegments = 16;
	double radialStart = 0.0;
	double radialLength = std::numbers::pi * 2.0;
	double tubularStart = 0.0;
	double tubularLength = std::numbers::pi * 2.0;
};
