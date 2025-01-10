//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "nlohmann/json_fwd.hpp"

#include "OtPolyhedronPrimitive.h"


//
//	OtIcosahedronPrimitive
//

class OtIcosahedronPrimitive : public OtPolyhedronPrimitive {
public:
	// constructor
	OtIcosahedronPrimitive();

	// serialize geometry
	nlohmann::json serialize(std::string* basedir) override;

	// get type name of primitive
	const char* getTypeName() override { return name; }

	// primitive name
	static constexpr char const* name = "Icosahedron";
};
