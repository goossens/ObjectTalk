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
	// initialize geometry
	OtObject init(size_t count, OtObject* parameters);

	// update attributes
	OtObject setTopRadius(double radius);
	OtObject setBottomRadius(double radius);
	OtObject setHeight(double height);
	OtObject setRadialSegments(int radialSegments);
	OtObject setHeightSegments(int heightSegments);
	OtObject setOpenEnded(bool openEnded);
	OtObject setThetaStart(double thetaStart);
	OtObject setThetaLength(double thetaLength);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtCylinder create();

private:
	// generate geometry
	void fillGeometry();

	// generate parts
	void generateTorso();
	void generateCap(bool top);

	// geometry
	double topRadius = 1.0;
	double bottomRadius = 1.0;
	double height = 1.0;
	int radialSegments = 16;
	int heightSegments = 1;
	bool openEnded = false;
	float thetaStart = 0.0;
	float thetaLength = std::numbers::pi * 2.0;
};
