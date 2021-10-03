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
//	OtSphereGeometryClass
//

class OtSphereGeometryClass;
typedef std::shared_ptr<OtSphereGeometryClass> OtSphereGeometry;

class OtSphereGeometryClass : public OtGeometryClass {
public:
	// initialize geometry
	OtObject init(size_t count, OtObject* parameters);

	// update attributes
	OtObject setRadius(float radius);
	OtObject setWidthSegments(int segments);
	OtObject setHeightSegments(int segments);
	OtObject setPhiStart(float phiStart);
	OtObject setPhiLength(float phiLength);
	OtObject setThetaStart(float thetaStart);
	OtObject setThetaLength(float thetaLength);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtSphereGeometry create();

private:
	// generate geometry
	void fillGeometry();

	// attributes
	float radius = 1.0;
	int widthSegments = 32;
	int heightSegments = 16;
	float phiStart = 0.0;
	float phiLength = std::numbers::pi * 2.0;
	float thetaStart = 0.0;
	float thetaLength = std::numbers::pi;
};
