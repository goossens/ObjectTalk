//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <glm/gtc/matrix_transform.hpp>
#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtUi.h"

#include "OtPathTools.h"
#include "OtSkySphereComponent.h"


//
//	OtSkySphereComponent::renderUI
//

bool OtSkySphereComponent::renderUI() {
	bool changed = false;
	changed |= texture.renderUI("Image");
	changed |= ImGui::SliderFloat("Brightness", &brightness, 0.1f, 4.0f, "%.1f");
	changed |= ImGui::SliderFloat("Gamma", &gamma, 0.5f, 3.0f, "%.1f");
	return changed;
}


//
//	OtSkySphereComponent::serialize
//

nlohmann::json OtSkySphereComponent::serialize(std::string* basedir) {
	auto data = nlohmann::json::object();
	data["component"] = name;
	data["texture"] = OtPathRelative(texture.getPath(), basedir);
	data["brightness"] = brightness;
	data["gamma"] = gamma;
	return data;
}


//
//	OtSkySphereComponent::deserialize
//

void OtSkySphereComponent::deserialize(nlohmann::json data, std::string* basedir) {
	texture = OtPathGetAbsolute(data, "texture", basedir);
	brightness = data.value("brightness", 1.0f);
	gamma = data.value("gamma", 2.2f);
}
