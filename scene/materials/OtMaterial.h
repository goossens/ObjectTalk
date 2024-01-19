//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>

#include "nlohmann/json_fwd.hpp"

#include "OtObject.h"


//
//	OtMaterial
//

class OtMaterialClass;
using OtMaterial = OtObjectPointer<OtMaterialClass>;

class OtMaterialClass : public OtObjectClass {
public:
	// UI to change material properties
	virtual bool renderUI() { return false; }

	// (de)serialize material
	virtual nlohmann::json serialize(std::string* basedir);
	virtual void deserialize(nlohmann::json data, std::string* basedir);

	// get type name of material
	virtual const char* getTypeName() { return name; }

	// material name
	static constexpr char const* name = "Material";

	// get type definition
	static OtType getMeta();
};
