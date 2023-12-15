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

#include "OtPathTools.h"
#include "OtWaterComponent.h"


//
//	OtWaterComponent::renderGUI
//

bool OtWaterComponent::renderGUI() {
	bool changed = false;

	changed |= ImGui::DragFloat("Level", &level, 1.0f, -100.0f, 1000.0f);
	changed |= ImGui::DragFloat("Distance", &distance, 1.0f, 1.0f, 1000.0f);
	changed |= ImGui::Checkbox("Use Refractance", &useRefractance);

	if (!useRefractance) {
		changed |= ImGui::ColorEdit3("Color", glm::value_ptr(color));
	}

	changed |= normals.renderGUI("Normals");
	changed |= ImGui::DragFloat("Scale", &scale, 0.01f, 0.01f, 100.0f);
	changed |= ImGui::DragFloat("Speed", &speed, 0.01f, 0.01f, 10.0f);
	changed |= ImGui::DragFloat("Metallic", &metallic, 0.01f, 0.0f, 1.0f);
	changed |= ImGui::DragFloat("Roughness", &roughness, 0.01f, 0.0f, 1.0f);
	changed |= ImGui::DragFloat("Ambient Occlusion", &ao, 0.01f, 0.0f, 1.0f);
	changed |= ImGui::DragFloat("Reflectivity", &reflectivity, 0.01f, 0.1f, 10.0f);

#if OT_DEBUG
	int width = ImGui::CalcItemWidth();
	int height = (width * 9) / 15;

	if (reflectionTextureIndex != OtTexture::invalidIndex) {
		ImGui::Image((void*)(intptr_t) reflectionTextureIndex, ImVec2(width, height));
		ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
		ImGui::TextUnformatted("Reflection");
	}

	if (refractionTextureIndex != OtTexture::invalidIndex) {
		ImGui::Image((void*)(intptr_t) refractionTextureIndex, ImVec2(width, height));
		ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
		ImGui::TextUnformatted("Refraction");
	}
#endif

	return changed;
}


//
//	OtWaterComponent::serialize
//

nlohmann::json OtWaterComponent::serialize(std::filesystem::path* basedir) {
	auto data = nlohmann::json::object();
	data["component"] = name;
	data["level"] = level;
	data["distance"] = distance;
	data["useRefractance"] = useRefractance;
	data["color"] = color;
	data["normals"] = OtPathGetRelative(normals.getPath(), basedir);
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

void OtWaterComponent::deserialize(nlohmann::json data, std::filesystem::path* basedir) {
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
