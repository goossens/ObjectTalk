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
#include "OtUi.h"

#include "OtDirectionalLightComponent.h"


//
//	OtDirectionalLightComponent::renderUI
//

bool OtDirectionalLightComponent::renderUI() {
	auto changed = false;
	changed |= OtUi::dragFloat("Elevation", &elevation, -90.0f, 90.0f);
	changed |= OtUi::dragFloat("Azimuth", &azimuth, 0.0f, 360.0f);
	changed |= ImGui::ColorEdit3("Color", glm::value_ptr(color));
	changed |= OtUi::dragFloat("Ambient", &ambient, 0.0f, 0.3f);
	changed |= OtUi::toggleButton("Cast Shadow", &castShadow);
	return changed;
}


//
//	OtDirectionalLightComponent::serialize
//

nlohmann::json OtDirectionalLightComponent::serialize([[maybe_unused]] std::string* basedir) {
	auto data = nlohmann::json::object();
	data["elevation"] = elevation;
	data["azimuth"] = azimuth;
	data["color"] = color;
	data["ambient"] = ambient;
	data["castShadow"] = castShadow;
	return data;
}


//
//	OtDirectionalLightComponent::deserialize
//

void OtDirectionalLightComponent::deserialize(nlohmann::json data, [[maybe_unused]] std::string* basedir) {
	elevation = data.value("elevation", 10.0f);
	azimuth = data.value("azimuth", 180.0f);
	color = data.value("color", glm::vec3(1.0f));
	ambient = data.value("ambient", 0.05f);
	castShadow = data.value("castShadow", false);
}


//
//	OtDirectionalLightComponent::getDirectionToLight
//

glm::vec3 OtDirectionalLightComponent::getDirectionToLight() {
	float elevationRad = glm::radians(elevation);
	float azimuthRad = glm::radians(azimuth);

	return glm::normalize(glm::vec3(
		std::cos(elevationRad) * std::sin(azimuthRad),
		std::sin(elevationRad),
		std::cos(elevationRad) * -std::cos(azimuthRad)));
}
