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

#include "OtMesh.h"


//
//	OtPrimitiveBase
//

class OtPrimitiveBase {
public:
	// subclasses need to override this
	virtual void createMesh(OtMesh* mesh) = 0;

	// UI to change primitive's properties
	virtual bool renderUI() = 0;

	// (de)serialize primitive
	virtual nlohmann::json serialize(std::string* basedir) = 0;
	virtual void deserialize(nlohmann::json data, std::string* basedir) = 0;

	// get type name of primitive
	virtual const char* getTypeName() { return name; }

	// primitive name
	static constexpr char const* name = "";
};
