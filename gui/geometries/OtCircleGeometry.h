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
//	OtCircleGeometryClass
//

class OtCircleGeometryClass;
using OtCircleGeometry = OtObjectPointer<OtCircleGeometryClass>;

class OtCircleGeometryClass : public OtGeometryClass {
public:
	// initialize geometry
	void init(size_t count, OtObject* parameters);

	// update attributes
	OtObject setRadius(float radius);
	OtObject setSegments(int segments);
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
	static constexpr char const* name = "Circle";

	// get type definition
	static OtType getMeta();

private:
	// generate geometry
	void fillGeometry() override;

	// geometry
	float radius = 1.0f;
	int segments = 16;
	float thetaStart = 0.0f;
	float thetaLength = std::numbers::pi * 2.0f;
};
