//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
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

#include "OtBackgroundComponent.h"


//
//	OtBackgroundComponent::renderUI
//

bool OtBackgroundComponent::renderUI() {
	return ImGui::ColorEdit3("Color", glm::value_ptr(color));
}


//
//	OtBackgroundComponent::serialize
//

nlohmann::json OtBackgroundComponent::serialize([[maybe_unused]] std::string* basedir) {
	auto data = nlohmann::json::object();
	data["color"] = color;
	return data;
}


//
//	OtBackgroundComponent::deserialize
//

void OtBackgroundComponent::deserialize(nlohmann::json data, [[maybe_unused]] std::string* basedir) {
	color = data.value("color", glm::vec3(0.0f));
}
