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
//	OtSphereClass
//

class OtSphereClass;
typedef std::shared_ptr<OtSphereClass> OtSphere;

class OtSphereClass : public OtGeometryClass {
public:
	// initialize geometry
	OtObject init(size_t count, OtObject* parameters);

	// update attributes
	OtObject setRadius(double radius);
	OtObject setWidthSegments(int segments);
	OtObject setHeightSegments(int segments);
	OtObject setPhiStart(double phiStart);
	OtObject setPhiLength(double phiLength);
	OtObject setThetaStart(double thetaStart);
	OtObject setThetaLength(double thetaLength);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtSphere create();

private:
	// generate geometry
	void fillGeometry();

	// attributes
	double radius = 1.0;
	int widthSegments = 32;
	int heightSegments = 16;
	double phiStart = 0.0;
	double phiLength = std::numbers::pi * 2.0;
	double thetaStart = 0.0;
	double thetaLength = std::numbers::pi;
};
