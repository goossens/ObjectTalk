//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtGlm.h"

#include "OtDirectionalLightComponent.h"


//
//	OtDirectionalLightComponent::renderUI
//

bool OtDirectionalLightComponent::renderUI() {
	bool changed = false;
	changed |= ImGui::ColorEdit3("Color", glm::value_ptr(color));
	changed |= ImGui::SliderFloat("Ambient", &ambient, 0.0f, 0.3f);
	return changed;
}


//
//	OtDirectionalLightComponent::serialize
//

nlohmann::json OtDirectionalLightComponent::serialize(std::string* basedir) {
	auto data = nlohmann::json::object();
	data["component"] = name;
	data["color"] = color;
	data["ambient"] = ambient;
	return data;
}


//
//	OtDirectionalLightComponent::deserialize
//

void OtDirectionalLightComponent::deserialize(nlohmann::json data, std::string* basedir) {
	color = data.value("color", glm::vec3(1.0f));
	ambient = data.value("ambient", 0.05f);
}
