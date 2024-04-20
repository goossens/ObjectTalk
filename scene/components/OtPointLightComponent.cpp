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
#include "OtUi.h"

#include "OtPointLightComponent.h"


//
//	OtPointLightComponent::renderUI
//

bool OtPointLightComponent::renderUI() {
	bool changed = false;
	changed |= OtUiEditVec3("Offset", &offset);
	changed |= OtUiDragFloat("Radius", &radius, 0.0f);
	changed |= ImGui::ColorEdit3("Color", glm::value_ptr(color));
	return changed;
}


//
//	OtPointLightComponent::serialize
//

nlohmann::json OtPointLightComponent::serialize(std::string* basedir) {
	auto data = nlohmann::json::object();
	data["component"] = name;
	data["offset"] = offset;
	data["radius"] = radius;
	data["color"] = color;
	return data;
}


//
//	OtPointLightComponent::deserialize
//

void OtPointLightComponent::deserialize(nlohmann::json data, std::string* basedir) {
	offset = data.value("offset", glm::vec3(0.0f));
	radius = data.value("radius", 1.0f);
	color = data.value("color", glm::vec3(1.0f));
}
