//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "nlohmann/json.hpp"

#include "OtNodesPin.h"


//
//	OtNodesPinClass::serialize
//

nlohmann::json OtNodesPinClass::serialize(std::string* basedir) {
	auto data = nlohmann::json::object();
	data["type"] = getTypeName();
	data["id"] = id;
	data["name"] = name;
	return data;
}


//
//	OtNodesPinClass::deserialize
//

void OtNodesPinClass::deserialize(nlohmann::json data, bool restoreIDs, std::string* basedir) {
	// restore ID (if required)
	if (restoreIDs) {
		id = data["id"];
	}
}
