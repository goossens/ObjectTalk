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

#include "OtUi.h"

#include "OtModelComponent.h"


//
//	OtModelComponent::renderUI
//

bool OtModelComponent::renderUI() {
	bool changed = false;
	changed |= model.renderUI("Model");
	changed |= OtUi::toggleButton("Cast shadow", &castShadow);
	return changed;
}


//
//	OtModelComponent::update
//

void OtModelComponent::update() {
	if (model.isReady()) {
		model->getModel().update();
	}
}


//
//	OtModelComponent::serialize
//

nlohmann::json OtModelComponent::serialize(std::string* basedir) {
	auto data = nlohmann::json::object();
	data["component"] = name;
	data["model"] = OtAssetSerialize(model.getPath(), basedir);
	data["castShadow"] = castShadow;
	return data;
}


//
//	OtModelComponent::deserialize
//

void OtModelComponent::deserialize(nlohmann::json data, std::string* basedir) {
	model = OtAssetDeserialize(&data, "model", basedir);
	castShadow = data.value("castShadow", true);
}
