//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtNumbers.h"

#include "OtGeometry.h"


//
//	OtTorusGeometryClass
//

class OtTorusGeometryClass;
typedef std::shared_ptr<OtTorusGeometryClass> OtTorusGeometry;

class OtTorusGeometryClass : public OtGeometryClass {
public:
	// initialize geometry
	void init(size_t count, OtObject* parameters);

	// update attributes
	OtObject setRadius(float radius);
	OtObject setTubeRadius(float tubeRadius);
	OtObject setRadialSegments(int segments);
	OtObject setTubularSegments(int segments);
	OtObject setRadialStart(float radialStart);
	OtObject setRadialLength(float radialLength);
	OtObject setTubularStart(float tubularStart);
	OtObject setTubularLength(float tubularLength);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtTorusGeometry create();

private:
	// generate geometry
	void fillGeometry() override;

	// attributes
	float radius = 1.0;
	float tubeRadius = 0.4;
	int radialSegments = 32;
	int tubularSegments = 16;
	float radialStart = 0.0;
	float radialLength = std::numbers::pi * 2.0;
	float tubularStart = 0.0;
	float tubularLength = std::numbers::pi * 2.0;
};
