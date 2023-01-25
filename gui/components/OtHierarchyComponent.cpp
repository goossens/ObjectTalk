//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "nlohmann/json.hpp"

#include "OtHierarchyComponent.h"


//
//	OtHierarchyComponent::serialize
//

nlohmann::json OtHierarchyComponent::serialize() {
	auto data = nlohmann::json::object();
	data["component"] = name;
	return data;
}


//
//	OtHierarchyComponent::deserialize
//

void OtHierarchyComponent::deserialize(nlohmann::json data) {
}
