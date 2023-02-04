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
//	OtCylinderGeometryClass
//

class OtCylinderGeometryClass;
typedef std::shared_ptr<OtCylinderGeometryClass> OtCylinderGeometry;

class OtCylinderGeometryClass : public OtGeometryClass {
public:
	// initialize geometry
	void init(size_t count, OtObject* parameters);

	// update attributes
	OtObject setTopRadius(float radius);
	OtObject setBottomRadius(float radius);
	OtObject setHeight(float height);
	OtObject setRadialSegments(int radialSegments);
	OtObject setHeightSegments(int heightSegments);
	OtObject setOpenEnded(bool openEnded);
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
	static constexpr char const* name = "Cylinder";

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtCylinderGeometry create();

private:
	// generate geometry
	void fillGeometry() override;

	// generate parts
	void generateTorso();
	void generateCap(bool top);

	// geometry
	float topRadius = 1.0f;
	float bottomRadius = 1.0f;
	float height = 1.0f;
	int radialSegments = 16;
	int heightSegments = 1;
	bool openEnded = false;
	float thetaStart = 0.0f;
	float thetaLength = std::numbers::pi * 2.0f;
};
