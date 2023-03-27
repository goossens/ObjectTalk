//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <filesystem>

#include "nlohmann/json_fwd.hpp"

#include "OtObject.h"


//
//	OtMaterial
//

class OtMaterialClass;
using OtMaterial = OtObjectPointer<OtMaterialClass>;

class OtMaterialClass : public OtObjectClass {
public:
	// GUI to change geometry properties
	virtual bool renderGUI() { return false; }

	// (de)serialize geometry
	virtual nlohmann::json serialize(std::filesystem::path* basedir);
	virtual void deserialize(nlohmann::json data, std::filesystem::path* basedir);

	// get type name of geometry
	virtual const char* getTypeName() { return name; }

	// material name
	static constexpr char const* name = "Material";

	// get type definition
	static OtType getMeta();
};
