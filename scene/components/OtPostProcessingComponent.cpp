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

#include "OtPostProcessingComponent.h"


//
//	OtPostProcessingComponent::renderGUI
//

bool OtPostProcessingComponent::renderGUI() {
	bool changed;
	changed |= ImGui::SliderFloat("Bloom Intensity", &bloomIntensity, 0.1f, 3.0f, "%.2f");
	changed |= ImGui::SliderFloat("Exposure", &exposure, 0.1f, 3.0f, "%.2f");
	changed |= ImGui::Checkbox("Fog Enabled", &fogEnabled);
	changed |= ImGui::ColorEdit3("Fog Color", glm::value_ptr(fogColor));
	changed |= ImGui::DragFloat("Fog Near", &fogNear, 1.0f, 0.0f, 0.0f, "%.1f");
	changed |= ImGui::DragFloat("Fog Far", &fogFar, 1.0f, 0.0f, 0.0f, "%.1f");
	return changed;
}


//
//	OtPostProcessingComponent::serialize
//

nlohmann::json OtPostProcessingComponent::serialize(std::filesystem::path* basedir) {
	auto data = nlohmann::json::object();
	data["component"] = name;
	data["bloomIntensity"] = bloomIntensity;
	data["Exposure"] = exposure;
	data["fogEnabled"] = fogEnabled;
	data["fogColor"] = fogColor;
	data["fogNear"] = fogNear;
	data["fogFar"] = fogFar;
	return data;
}


//
//	OtPostProcessingComponent::deserialize
//

void OtPostProcessingComponent::deserialize(nlohmann::json data, std::filesystem::path* basedir) {
	bloomIntensity = data.value("bloomIntensity", 0.8f);
	exposure = data.value("Exposure", 1.0f);
	fogEnabled = data.value("fogEnabled", false);
	fogColor = data.value("fogColor", glm::vec3(1.0f));
	fogNear = data.value("fogNear", 1.0f);
	fogFar = data.value("fogFar", 1000.0f);
}
