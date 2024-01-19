//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
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
//	OtPostProcessingComponent::renderUI
//

bool OtPostProcessingComponent::renderUI() {
	bool changed;
	changed |= ImGui::SliderFloat("Bloom Intensity", &bloomIntensity, 0.0f, 3.0f, "%.2f");
	changed |= ImGui::SliderFloat("Exposure", &exposure, 0.1f, 3.0f, "%.2f");
	return changed;
}


//
//	OtPostProcessingComponent::serialize
//

nlohmann::json OtPostProcessingComponent::serialize(std::string* basedir) {
	auto data = nlohmann::json::object();
	data["component"] = name;
	data["bloomIntensity"] = bloomIntensity;
	data["Exposure"] = exposure;
	return data;
}


//
//	OtPostProcessingComponent::deserialize
//

void OtPostProcessingComponent::deserialize(nlohmann::json data, std::string* basedir) {
	bloomIntensity = data.value("bloomIntensity", 0.8f);
	exposure = data.value("Exposure", 1.0f);
}
