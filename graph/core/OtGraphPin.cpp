//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "nlohmann/json.hpp"

#include "OtGraphPin.h"


//
//	Constants
//

static constexpr const char* pinTypeNames[] = { "bool", "float" };


//
//	OtGraphPinClass::serialize
//

nlohmann::json OtGraphPinClass::serialize(std::filesystem::path* basedir) {
	auto data = nlohmann::json::object();
	data["type"] = getTypeName();
	data["id"] = id;
	data["name"] = name;
	return data;
}


//
//	OtGraphPinClass::deserialize
//

void OtGraphPinClass::deserialize(nlohmann::json data, bool restoreIDs, std::filesystem::path* basedir) {
	// restore ID (if required)
	if (restoreIDs) {
		id = data["id"];
	}
}
