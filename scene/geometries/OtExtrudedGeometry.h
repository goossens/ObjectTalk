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
#include "OtShape.h"


//
//	OtExtrudedGeometryClass
//

class OtExtrudedGeometryClass;
using OtExtrudedGeometry = OtObjectPointer<OtExtrudedGeometryClass>;

class OtExtrudedGeometryClass : public OtGeometryClass {
public:
	// initialize geometry
	void init(size_t count, OtObject* parameters);

	// update attributes
	OtObject setShape(OtObject shape);
	OtObject setDepth(float depth);
	OtObject setSegments(int segments);

	// GUI to change geometry properties
	bool renderGUI() override;

	// (de)serialize geometry
	nlohmann::json serialize(std::filesystem::path* basedir) override;
	void deserialize(nlohmann::json data, std::filesystem::path* basedir) override;

	// get type name of geometry
	const char* getTypeName() override { return name; }

	// geometry name
	static constexpr char const* name = "Extruded";

	// get type definition
	static OtType getMeta();

private:
	// generate geometry
	void fillGeometry() override;

	// attributes
	OtShape shape;
	float depth = 1.0;
	int segments = 12;
};
