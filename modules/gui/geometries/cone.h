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
	// initialize geometry
	OtObject init(size_t count, OtObject* parameters);

	// update attributes
	OtObject setRadius(double radius);
	OtObject setHeight(double height);
	OtObject setSegments(int segments);
	OtObject setPartial(double thetaStart, double thetaLength);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtCone create();
	static OtCone create(double radius, double height, long segments);

private:
	// generate geometry
	void fillBuffers();

	// geometry
	double radius = 1.0;
	double height = 1.0;
	int segments = 20;
	float thetaStart = 0.0;
	float thetaLength = std::numbers::pi * 2.0;
};
