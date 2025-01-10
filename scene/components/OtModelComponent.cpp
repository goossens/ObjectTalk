//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
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

nlohmann::json OtModelComponent::serialize(std::string* basedir) {
	auto data = nlohmann::json::object();
	data["component"] = name;
	data["model"] = OtAssetSerialize(model.getPath(), basedir);
	return data;
}


//
//	OtModelComponent::deserialize
//

void OtModelComponent::deserialize(nlohmann::json data, std::string* basedir) {
	model = OtAssetDeserialize(&data, "model", basedir);
}
