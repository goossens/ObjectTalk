//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtUi.h"
#include "OtTagComponent.h"


//
//	OtTagComponent::renderGUI
//

bool OtTagComponent::renderGUI() {
	return OtUiInputText("Name", tag);
}


//
//	OtTagComponent::serialize
//

nlohmann::json OtTagComponent::serialize(std::filesystem::path* basedir) {
	auto data = nlohmann::json::object();
	data["component"] = name;
	data["tag"] = tag;
	return data;
}


//
//	OtTagComponent::deserialize
//

void OtTagComponent::deserialize(nlohmann::json data, std::filesystem::path* basedir) {
	tag = data["tag"];
}
