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

#include "OtPostProcessingComponent.h"


//
//	OtPostProcessingComponent::renderUI
//

bool OtPostProcessingComponent::renderUI() {
	bool changed = false;
	changed |= OtUi::toggleButton("FXAA", &fxaa);
	changed |= OtUi::toggleButton("Fog", &fog);

	if (fog) {
		changed |= OtUi::dragFloat("Fog Density", &fogDensity);
		changed |= OtUi::dragFloat("Background Fog", &backgroundFogRatio, 0.0f, 1.0f);
		changed |= ImGui::ColorEdit3("Fog Color", glm::value_ptr(fogColor));
	}

	changed |= OtUi::toggleButton("Bloom", &bloom);

	if (bloom) {
		changed |= OtUi::dragFloat("Bloom Intensity", &bloomIntensity, 0.0f, 3.0f);
	}

	changed |= OtUi::toggleButton("Godrays", &godrays);
	changed |= OtUi::dragFloat("Exposure", &exposure, 0.1f, 3.0f);
	changed |= OtUi::dragFloat("Contrast", &contrast, 0.5f, 2.0f);

	changed |= OtUi::selectorEnum("Tonemap", &tonemap, OtPostProcessing::tonemapTypes, OtPostProcessing::tonemapTypeCount);

	return changed;
}


//
//	OtPostProcessingComponent::serialize
//

nlohmann::json OtPostProcessingComponent::serialize([[maybe_unused]] std::string* basedir) {
	auto data = nlohmann::json::object();
	data["fxaa"] = fxaa;
	data["fog"] = fog;
	data["fogDensity"] = fogDensity;
	data["backgroundFogRatio"] = backgroundFogRatio;
	data["fogColor"] = fogColor;
	data["bloom"] = bloom;
	data["bloomIntensity"] = bloomIntensity;
	data["godrays"] = godrays;
	data["exposure"] = exposure;
	data["contrast"] = contrast;
	data["tonemap"] = tonemap;
	return data;
}


//
//	OtPostProcessingComponent::deserialize
//

void OtPostProcessingComponent::deserialize(nlohmann::json data, [[maybe_unused]] std::string* basedir) {
	fxaa = data.value("fxaa", false);
	fog = data.value("fog", false);
	fogDensity = data.value("fogDensity", 0.1f);
	backgroundFogRatio = data.value("backgroundFogRatio", 1.0f);
	fogColor = data.value("fogColor", glm::vec3(0.4f, 0.5f, 0.8f));
	bloom = data.value("bloom", false);
	bloomIntensity = data.value("bloomIntensity", 0.8f);
	godrays = data.value("godrays", false);
	exposure = data.value("exposure", 1.0f);
	contrast = data.value("contrast", 1.0f);
	tonemap = data.value("tonemap", OtPostProcessing::Tonemap::acesFilmic);
}
