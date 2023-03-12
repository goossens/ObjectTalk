//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "nlohmann/json_fwd.hpp"

#include "OtGeometry.h"


//
//	OtTorusGeometryClass
//

class OtTorusGeometryClass;
using OtTorusGeometry = OtObjectPointer<OtTorusGeometryClass>;

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

	// GUI to change geometry properties
	bool renderGUI() override;

	// (de)serialize geometry
	nlohmann::json serialize() override;
	void deserialize(nlohmann::json data) override;

	// get type name of geometry
	const char* getTypeName() override { return name; }

	// geometry name
	static constexpr char const* name = "Torus";

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtTorusGeometry create();

private:
	// generate geometry
	void fillGeometry() override;

	// attributes
	float radius = 1.0f;
	float tubeRadius = 0.4f;
	int radialSegments = 64;
	int tubularSegments = 32;
	float radialStart = 0.0f;
	float radialLength = 360.0f;
	float tubularStart = 0.0f;
	float tubularLength = 360.0f;
};
