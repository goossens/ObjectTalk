//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "nlohmann/json.hpp"

#include "OtGeometryComponent.h"


//
//	OtGeometryComponent::renderGUI
//

bool OtGeometryComponent::renderGUI() {
	return false;
}


//
//	OtGeometryComponent::serialize
//

nlohmann::json OtGeometryComponent::serialize() {
	auto data = nlohmann::json::object();
	data["component"] = name;
	return data;
}


//
//	OtGeometryComponent::deserialize
//

void OtGeometryComponent::deserialize(nlohmann::json data) {
}
