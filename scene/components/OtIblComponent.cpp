//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtUi.h"

#include "OtPathTools.h"
#include "OtIblComponent.h"


//
//	OtIblComponent::renderUI
//

bool OtIblComponent::renderUI() {
	bool changed = false;
	changed |= cubemap.renderUI("Cube Map");
	return changed;
}


//
//	OtIblComponent::serialize
//

nlohmann::json OtIblComponent::serialize(std::string* basedir) {
	auto data = nlohmann::json::object();
	data["component"] = name;
	data["cubemap"] = OtPathRelative(cubemap.getPath(), basedir);
	return data;
}


//
//	OtIblComponent::deserialize
//

void OtIblComponent::deserialize(nlohmann::json data, std::string* basedir) {
	cubemap = OtPathGetAbsolute(data, "cubemap", basedir);
}
