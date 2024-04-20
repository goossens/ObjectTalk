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

#include "OtPostProcessingComponent.h"


//
//	OtPostProcessingComponent::renderUI
//

bool OtPostProcessingComponent::renderUI() {
	bool changed;
	changed |= OtUiToggleButton("FXAA", &fxaa);
	changed |= OtUiToggleButton("Fog", &fog);

	if (fog) {
		changed |= OtUiDragFloat("Fog Density", &fogDensity);
		changed |= ImGui::ColorEdit3("Fog Color", glm::value_ptr(fogColor));
	}

	changed |= OtUiDragFloat("Bloom Intensity", &bloomIntensity, 0.0f, 3.0f);

	changed |= OtUiToggleButton("Godrays", &godrays);
	changed |= OtUiDragFloat("Exposure", &exposure, 0.1f, 3.0f);
	changed |= OtUiDragFloat("Contrast", &contrast, 0.5f, 2.0f);
	return changed;
}


//
//	OtPostProcessingComponent::serialize
//

nlohmann::json OtPostProcessingComponent::serialize(std::string* basedir) {
	auto data = nlohmann::json::object();
	data["fxaa"] = fxaa;
	data["fog"] = fog;
	data["fogDensity"] = fogDensity;
	data["fogColor"] = fogColor;
	data["bloomIntensity"] = bloomIntensity;
	data["godrays"] = godrays;
	data["exposure"] = exposure;
	data["contrast"] = contrast;
	return data;
}


//
//	OtPostProcessingComponent::deserialize
//

void OtPostProcessingComponent::deserialize(nlohmann::json data, std::string* basedir) {
	fxaa = data.value("fxaa", false);
	bloomIntensity = data.value("bloomIntensity", 0.8f);
	fog = data.value("fog", false);
	fogDensity = data.value("fogDensity", 0.1f);
	fogColor = data.value("fogColor", glm::vec3(0.4f, 0.5f, 0.8f));
	godrays = data.value("godrays", false);
	exposure = data.value("exposure", 1.0f);
	contrast = data.value("contrast", 1.0f);
}
