//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
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

#include "OtSkyComponent.h"


//
//	OtSkyComponent::renderGUI
//

bool OtSkyComponent::renderGUI() {
	bool changed = false;
	changed |= ImGui::SliderFloat("Sun Elevation", &elevation, -90.0f, 90.0f);
	changed |= ImGui::SliderFloat("Sun Azimuth", &azimuth, 0.0f, 360.0f);

	changed |= ImGui::SliderFloat("Rayleigh Coefficient", &rayleighCoefficient, 0.5f, 6.0f);
	changed |= ImGui::SliderFloat("Mie Coefficient", &mieCoefficient, 1.0f, 10.0f);
	changed |= ImGui::SliderFloat("Mie Scattering", &mieScattering, 0.9f, 0.99f);

	changed |= ImGui::SliderFloat("Cirrus Clouds", &cirrus, 0.0f, 1.0f);
	changed |= ImGui::SliderFloat("Cumulus Clouds", &cumulus, 0.0f, 1.0f);
	changed |= ImGui::SliderFloat("Cloud Speed", &speed, 0.0f, 10.0f);
	return changed;
}


//
//	OtSkyComponent::serialize
//

nlohmann::json OtSkyComponent::serialize(std::filesystem::path* basedir) {
	auto data = nlohmann::json::object();
	data["component"] = name;
	data["elevation"] = elevation;
	data["azimuth"] = azimuth;
	data["rayleighCoefficient"] = rayleighCoefficient;
	data["mieCoefficient"] = mieCoefficient;
	data["mieScattering"] = mieScattering;
	data["cirrus"] = cirrus;
	data["cumulus"] = cumulus;
	data["speed"] = speed;
	return data;
}


//
//	OtSkyComponent::deserialize
//

void OtSkyComponent::deserialize(nlohmann::json data, std::filesystem::path* basedir) {
	elevation = data.value("elevation", 10.0f);
	azimuth = data.value("azimuth", 180.0f);
	rayleighCoefficient = data.value("rayleighCoefficient", 3.5f);
	mieCoefficient = data.value("mieCoefficient", 5.0f);
	mieScattering = data.value("mieScattering", 0.99f);
	cirrus = data.value("cirrus", 0.4f);
	cumulus = data.value("cumulus", 0.8f);
	speed = data.value("speed", 2.0f);
}


//
//	OtSkyComponent::getDirectionToSun
//

glm::vec3 OtSkyComponent::getDirectionToSun() {
	float elevationRad = glm::radians(elevation);
	float azimuthRad = glm::radians(azimuth);

	return glm::normalize(glm::vec3(
		std::cos(elevationRad) * std::sin(azimuthRad),
		std::sin(elevationRad),
		std::cos(elevationRad) * -std::cos(azimuthRad)));
}


//
//	OtSkyComponent::getLightDirection
//

glm::vec3 OtSkyComponent::getLightDirection() {
	return -getDirectionToSun();
}
