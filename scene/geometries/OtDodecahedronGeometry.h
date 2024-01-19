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

#include "OtNumbers.h"

#include "OtPolyhedronGeometry.h"


//
//	OtDodecahedronGeometryClass
//

class OtDodecahedronGeometryClass;
using OtDodecahedronGeometry = OtObjectPointer<OtDodecahedronGeometryClass>;

class OtDodecahedronGeometryClass : public OtPolyhedronGeometryClass {
public:
	// constructor
	OtDodecahedronGeometryClass();

	// UI to change geometry properties
	bool renderUI() override;

	// serialize geometry
	nlohmann::json serialize(std::string* basedir) override;

	// get type name of geometry
	const char* getTypeName() override { return name; }

	// geometry name
	static constexpr char const* name = "Dodecahedron";

	// get type definition
	static OtType getMeta();

private:
};
