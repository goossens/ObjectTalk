//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "nlohmann/json.hpp"

#include "OtNameComponent.h"


//
//	OtNameComponent::serialize
//

nlohmann::json OtNameComponent::serialize() {
	auto data = nlohmann::json::object();
	data["type"] = "name";
	data["name"] = name;
	return data;
}


//
//	OtNameComponent::deserialize
//

void OtNameComponent::deserialize(nlohmann::json data) {
	name = data["name"];
}
