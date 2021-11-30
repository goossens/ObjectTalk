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
//	OtCylinderGeometryClass
//

class OtCylinderGeometryClass;
typedef std::shared_ptr<OtCylinderGeometryClass> OtCylinderGeometry;

class OtCylinderGeometryClass : public OtGeometryClass {
public:
	// initialize geometry
	OtObject init(size_t count, OtObject* parameters);

	// update attributes
	OtObject setTopRadius(float radius);
	OtObject setBottomRadius(float radius);
	OtObject setHeight(float height);
	OtObject setRadialSegments(int radialSegments);
	OtObject setHeightSegments(int heightSegments);
	OtObject setOpenEnded(bool openEnded);
	OtObject setThetaStart(float thetaStart);
	OtObject setThetaLength(float thetaLength);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtCylinderGeometry create();

private:
	// generate geometry
	void fillGeometry();

	// generate parts
	void generateTorso();
	void generateCap(bool top);

	// geometry
	float topRadius = 1.0;
	float bottomRadius = 1.0;
	float height = 1.0;
	int radialSegments = 16;
	int heightSegments = 1;
	bool openEnded = false;
	float thetaStart = 0.0;
	float thetaLength = std::numbers::pi * 2.0;
};