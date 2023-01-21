//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "nlohmann/json.hpp"

#include "OtUuidComponent.h"


//
//	OtUuidComponent::serialize
//

nlohmann::json OtUuidComponent::serialize() {
	auto data = nlohmann::json::object();
	data["type"] = "uuid";
	data["uuid"] = uuid;
	return data;
}


//
//	OtUuidComponent::deserialize
//

void OtUuidComponent::deserialize(nlohmann::json data) {
	uuid = data["uuid"];
}
