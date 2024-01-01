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
//	OtTetrahedronGeometryClass
//

class OtTetrahedronGeometryClass;
using OtTetrahedronGeometry = OtObjectPointer<OtTetrahedronGeometryClass>;

class OtTetrahedronGeometryClass : public OtPolyhedronGeometryClass {
public:
	// constructor
	OtTetrahedronGeometryClass();

	// (de)serialize geometry
	nlohmann::json serialize(std::filesystem::path* basedir) override;

	// get type name of geometry
	const char* getTypeName() override { return name; }

	// geometry name
	static constexpr char const* name = "Tetrahedron";

	// get type definition
	static OtType getMeta();

private:
};
