//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "nlohmann/json.hpp"

#include "OtComponent.h"


//
//	OtComponent::serializeToString
//

std::string OtComponent::serializeToString() {
	 return serialize().dump();
}


//
//	OtComponent::deserializeFromString
//

void OtComponent::deserializeFromString(const std::string& data) {
	return deserialize(nlohmann::json::parse(data));
}
