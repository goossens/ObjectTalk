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
//	OtCircleClass
//

class OtCircleClass;
typedef std::shared_ptr<OtCircleClass> OtCircle;

class OtCircleClass : public OtGeometryClass {
public:
	// initialize geometry
	OtObject init(size_t count, OtObject* parameters);

	// update attributes
	OtObject setRadius(double radius);
	OtObject setSegments(int segments);
	OtObject setPartial(double thetaStart, double thetaLength);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtCircle create();
	static OtCircle create(double radius, long segments);

private:
	// generate geometry
	void fillBuffers();

	// geometry
	double radius = 1.0;
	int segments = 8;
	double thetaStart = 0.0;
	double thetaLength = std::numbers::pi * 2.0;
};
