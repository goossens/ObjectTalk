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

#include "OtDirectionalLightComponent.h"


//
//	OtDirectionalLightComponent::renderGUI
//

bool OtDirectionalLightComponent::renderGUI() {
	return ImGui::ColorEdit3("Color", glm::value_ptr(color));
}


//
//	OtDirectionalLightComponent::serialize
//

nlohmann::json OtDirectionalLightComponent::serialize(std::filesystem::path* basedir) {
	auto data = nlohmann::json::object();
	data["component"] = name;
	data["color"] = color;
	return data;
}


//
//	OtDirectionalLightComponent::deserialize
//

void OtDirectionalLightComponent::deserialize(nlohmann::json data, std::filesystem::path* basedir) {
	color = data.value("color", glm::vec3(1.0f));
}
