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
//	OtCylinderGeometryClass
//

class OtCylinderGeometryClass;
using OtCylinderGeometry = OtObjectPointer<OtCylinderGeometryClass>;

class OtCylinderGeometryClass : public OtGeometryClass {
public:
	// initialize geometry
	void init(size_t count, OtObject* parameters);

	// update attributes
	OtObject setTopRadius(float radius);
	OtObject setBottomRadius(float radius);
	OtObject setRadialSegments(int radialSegments);
	OtObject setHeightSegments(int heightSegments);
	OtObject setOpenEnded(bool openEnded);
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
	static constexpr char const* name = "Cylinder";

	// get type definition
	static OtType getMeta();

private:
	// generate geometry
	void fillGeometry() override;

	// generate parts
	void generateTorso();
	void generateCap(bool top);

	// geometry
	float topRadius = 1.0f;
	float bottomRadius = 1.0f;
	int radialSegments = 32;
	int heightSegments = 1;
	bool openEnded = false;
	float thetaStart = 0.0f;
	float thetaLength = 360.0f;
};
