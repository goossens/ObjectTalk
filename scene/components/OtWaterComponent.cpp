//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "glm/ext.hpp"
#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtGlm.h"

#include "OtWaterComponent.h"


//
//	OtWaterComponent::renderGUI
//

bool OtWaterComponent::renderGUI() {
	bool changed = false;
	changed |= ImGui::DragFloat("Height", &height, 1.0f, -100.0f, 1000.0f);
	changed |= ImGui::ColorEdit3("Color", glm::value_ptr(color));
	changed |= ImGui::DragFloat("Shininess", &shininess, 0.01f, 0.0f, 1.0f);
	return changed;
}


//
//	OtWaterComponent::serialize
//

nlohmann::json OtWaterComponent::serialize(std::filesystem::path* basedir) {
	auto data = nlohmann::json::object();
	data["component"] = name;
	data["height"] = height;
	data["color"] = color;
	data["shininess"] = shininess;
	return data;
}


//
//	OtWaterComponent::deserialize
//

void OtWaterComponent::deserialize(nlohmann::json data, std::filesystem::path* basedir) {
	height = data.value("height", 0.0f);
	color = data.value("color", glm::vec3(0.5f, 0.6f, 0.8f));
	shininess = data.value("shininess", 0.5f);
}
