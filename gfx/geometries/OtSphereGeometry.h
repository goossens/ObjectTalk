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

#include "OtNumbers.h"

#include "OtGeometry.h"


//
//	OtSphereGeometryClass
//

class OtSphereGeometryClass;
using OtSphereGeometry = OtObjectPointer<OtSphereGeometryClass>;

class OtSphereGeometryClass : public OtGeometryClass {
public:
	// initialize geometry
	void init(size_t count, OtObject* parameters);

	// update attributes
	OtObject setRadius(float radius);
	OtObject setWidthSegments(int segments);
	OtObject setHeightSegments(int segments);
	OtObject setPhiStart(float phiStart);
	OtObject setPhiLength(float phiLength);
	OtObject setThetaStart(float thetaStart);
	OtObject setThetaLength(float thetaLength);

	// GUI to change geometry properties
	bool renderGUI() override;

	// (de)serialize geometry
	nlohmann::json serialize() override;
	void deserialize(nlohmann::json data) override;

	// get type name of geometry
	const char* getTypeName() override { return name; }

	// geometry name
	static constexpr char const* name = "Sphere";

	// get type definition
	static OtType getMeta();

private:
	// generate geometry
	void fillGeometry() override;

	// attributes
	float radius = 1.0f;
	int widthSegments = 32;
	int heightSegments = 16;
	float phiStart = 0.0f;
	float phiLength = std::numbers::pi * 2.0f;
	float thetaStart = 0.0f;
	float thetaLength = std::numbers::pi;
};
