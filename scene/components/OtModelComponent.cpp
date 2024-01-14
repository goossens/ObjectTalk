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

#include "OtException.h"

#include "OtUi.h"

#include "OtPathTools.h"
#include "OtModelComponent.h"


//
//	OtModelComponent::renderUI
//

bool OtModelComponent::renderUI() {
	return model.renderUI("Model");
}


//
//	OtModelComponent::serialize
//

nlohmann::json OtModelComponent::serialize(std::filesystem::path* basedir) {
	auto data = nlohmann::json::object();
	data["component"] = name;
	data["model"] = OtPathGetRelative(model.getPath(), basedir);
	return data;
}


//
//	OtModelComponent::deserialize
//

void OtModelComponent::deserialize(nlohmann::json data, std::filesystem::path* basedir) {
	model = OtPathGetAbsolute(data, "model", basedir);
}
