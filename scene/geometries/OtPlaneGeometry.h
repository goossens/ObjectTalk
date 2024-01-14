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
//	OtPlaneGeometryClass
//

class OtPlaneGeometryClass;
using OtPlaneGeometry = OtObjectPointer<OtPlaneGeometryClass>;

class OtPlaneGeometryClass : public OtGeometryClass {
public:
	// initialize geometry
	void init(size_t count, OtObject* parameters);

	// update attributes
	OtObject setWidthSegments(int widthSegments);
	OtObject setHeightSegments(int heightSegments);

	// get information
	int getWidthSegments() { return widthSegments; }
	int getHeightSegments() { return heightSegments; }

	// UI to change geometry properties
	bool renderUI() override;

	// (de)serialize geometry
	nlohmann::json serialize(std::filesystem::path* basedir) override;
	void deserialize(nlohmann::json data, std::filesystem::path* basedir) override;

	// get type name of geometry
	const char* getTypeName() override { return name; }

	// geometry name
	static constexpr char const* name = "Plane";

	// get type definition
	static OtType getMeta();

private:
	// generate geometry
	void fillGeometry() override;

	// properties
	int widthSegments = 1;
	int heightSegments = 1;
};
