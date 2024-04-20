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

#include "OtPathTools.h"
#include "OtWaterComponent.h"


//
//	OtWaterComponent::renderUI
//

bool OtWaterComponent::renderUI() {
	bool changed = false;

	changed |= OtUiDragFloat("Level", &level, -100.0f, 1000.0f);
	changed |= OtUiDragFloat("Distance", &distance, 1.0f, 1000.0f);
	changed |= OtUiToggleButton("Use Refractance", &useRefractance);

	if (!useRefractance) {
		changed |= ImGui::ColorEdit3("Color", glm::value_ptr(color));
	}

	changed |= normals.renderUI("Normals");
	changed |= OtUiDragFloat("Scale", &scale, 0.01f, 100.0f);
	changed |= OtUiDragFloat("Speed", &speed, 0.01f, 10.0f);
	changed |= OtUiDragFloat("Metallic", &metallic, 0.0f, 1.0f);
	changed |= OtUiDragFloat("Roughness", &roughness, 0.0f, 1.0f);
	changed |= OtUiDragFloat("Ambient Occlusion", &ao, 0.0f, 1.0f);
	changed |= OtUiDragFloat("Reflectivity", &reflectivity, 0.1f, 10.0f);
	return changed;
}


//
//	OtWaterComponent::serialize
//

nlohmann::json OtWaterComponent::serialize(std::string* basedir) {
	auto data = nlohmann::json::object();
	data["component"] = name;
	data["level"] = level;
	data["distance"] = distance;
	data["useRefractance"] = useRefractance;
	data["color"] = color;
	data["normals"] = OtPathRelative(normals.getPath(), basedir);
	data["scale"] = scale;
	data["speed"] = speed;
	data["metallic"] = metallic;
	data["roughness"] = roughness;
	data["ao"] = ao;
	data["reflectivity"] = reflectivity;
	return data;
}


//
//	OtWaterComponent::deserialize
//

void OtWaterComponent::deserialize(nlohmann::json data, std::string* basedir) {
	level = data.value("level", 0.0f);
	distance = data.value("distance", 100.0f);
	useRefractance = data.value("useRefractance", true);
	color = data.value("color", glm::vec3(0.5f, 0.6f, 0.8f));
	normals = OtPathGetAbsolute(data, "normals", basedir);
	scale = data.value("scale", 10.0f);
	speed = data.value("speed", 1.0f);
	metallic = data.value("metallic", 0.3f);
	roughness = data.value("roughness", 0.2f);
	ao = data.value("ao", 0.25f);
	reflectivity = data.value("reflectivity", 1.0f);
}
