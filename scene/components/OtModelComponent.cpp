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
	changed |= asset.renderUI("Model");
	changed |= OtUi::toggleButton("Cast shadow", &castShadow);

	if (asset.isReady()) {
		if (ImGui::Button("Open Details", ImVec2(ImGui::CalcItemWidth(), 0.0f))) {
			ImGui::OpenPopup("ModelPopup");
		}

		if (ImGui::BeginPopup("ModelPopup")) {
			asset->getModel().renderDetails();
			ImGui::EndPopup();
		}
	}

	return changed;
}


//
//	OtModelComponent::update
//

void OtModelComponent::update() {
	if (asset.isReady()) {
		asset->getModel().update();
	}
}


//
//	OtModelComponent::serialize
//

nlohmann::json OtModelComponent::serialize(std::string* basedir) {
	auto data = nlohmann::json::object();
	data["model"] = OtAssetSerialize(asset.getPath(), basedir);
	data["castShadow"] = castShadow;
	return data;
}


//
//	OtModelComponent::deserialize
//

void OtModelComponent::deserialize(nlohmann::json data, std::string* basedir) {
	asset = OtAssetDeserialize(&data, "model", basedir);
	castShadow = data.value("castShadow", true);
}
