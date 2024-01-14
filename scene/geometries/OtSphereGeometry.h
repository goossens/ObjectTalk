//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
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
	OtObject setPhiSegments(int segments);
	OtObject setPhiStart(float phiStart);
	OtObject setPhiLength(float phiLength);
	OtObject setThetaSegments(int segments);
	OtObject setThetaStart(float thetaStart);
	OtObject setThetaLength(float thetaLength);

	// UI to change geometry properties
	bool renderUI() override;

	// (de)serialize geometry
	nlohmann::json serialize(std::filesystem::path* basedir) override;
	void deserialize(nlohmann::json data, std::filesystem::path* basedir) override;

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
	int phiSegments = 32;
	float phiStart = 0.0f;
	float phiLength = 360.0f;
	int thetaSegments = 64;
	float thetaStart = 0.0f;
	float thetaLength = 180.0f;
};
