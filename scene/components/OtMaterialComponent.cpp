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
#include "OtUi.h"

#include "OtComponentTools.h"
#include "OtMaterialComponent.h"


//
//	OtMaterialComponent::renderGUI
//

static inline bool editPath(const char* label, std::filesystem::path& path, bool& flag) {
	bool changed = false;

	if (OtUiFileSelector(label, path)) {
		changed = true;
		flag = true;
	}

	return changed;
}

bool OtMaterialComponent::renderGUI() {
	bool changed = false;
	changed |= ImGui::ColorEdit3("Albedo Color", glm::value_ptr(albedo));
	changed |= ImGui::SliderFloat("Metallic", &metallic, 0.0f, 1.0f, "%.2f");
	changed |= ImGui::SliderFloat("Roughness", &roughness, 0.0f, 1.0f, "%.2f");
	changed |= ImGui::SliderFloat("Emissive", &emissive, 0.0f, 4.0f, "%.2f");
	changed |= ImGui::SliderFloat("Ambient Occlusion", &ao, 0.0f, 1.0f, "%.2f");
	changed |= editPath("Albedo Texture", albedoTexturePath, updateAlbedoTexture);
	changed |= editPath("Normal Texture", normalTexturePath, updateNormalTexture);
	changed |= editPath("Metallic Texture", metallicTexturePath, updateMetallicTexture);
	changed |= editPath("Roughness Texture", roughnessTexturePath, updateRoughnessTexture);
	changed |= editPath("Emissive Texture", emissiveTexturePath, updateEmissiveTexture);
	changed |= editPath("AO Texture", aoTexturePath, updateAoTexture);
	return changed;
}


//
//	OtMaterialComponent::serialize
//

nlohmann::json OtMaterialComponent::serialize(std::filesystem::path* basedir) {
	auto data = nlohmann::json::object();
	data["component"] = name;
	data["albedo"] = albedo;
	data["metallic"] = metallic;
	data["roughness"] = roughness;
	data["emissive"] = emissive;
	data["ao"] = ao;
	data["albedoTexture"] = OtComponentGetRelativePath(albedoTexturePath, basedir);
	data["normalTexture"] = OtComponentGetRelativePath(normalTexturePath, basedir);
	data["metallicTexture"] = OtComponentGetRelativePath(metallicTexturePath, basedir);
	data["roughnessTexture"] = OtComponentGetRelativePath(roughnessTexturePath, basedir);
	data["emissiveTexture"] = OtComponentGetRelativePath(emissiveTexturePath, basedir);
	data["aoTexture"] = OtComponentGetRelativePath(aoTexturePath, basedir);
	return data;
}


//
//	OtMaterialComponent::deserialize
//

void OtMaterialComponent::deserialize(nlohmann::json data, std::filesystem::path* basedir) {
	albedo = data.value("albedo", glm::vec3(1.0f));
	metallic = data.value("metallic", 0.5f);
	roughness = data.value("roughness", 0.5f);
	emissive = data.value("emissive", 0.0f);
	ao = data.value("ao", 1.0f);
	albedoTexturePath = OtComponentGetAbsolutePath(data, "albedoTexture", basedir);
	normalTexturePath = OtComponentGetAbsolutePath(data, "normalTexture", basedir);
	metallicTexturePath = OtComponentGetAbsolutePath(data, "metallicTexture", basedir);
	roughnessTexturePath = OtComponentGetAbsolutePath(data, "roughnessTexture", basedir);
	emissiveTexturePath = OtComponentGetAbsolutePath(data, "emissiveTexture", basedir);
	aoTexturePath = OtComponentGetAbsolutePath(data, "aoTexture", basedir);

	updateAlbedoTexture = !albedoTexturePath.empty();
	updateNormalTexture = !normalTexturePath.empty();
	updateMetallicTexture = !metallicTexturePath.empty();
	updateRoughnessTexture = !roughnessTexturePath.empty();
	updateEmissiveTexture = !emissiveTexturePath.empty();
	updateAoTexture = !aoTexturePath.empty();
}


//
//	OtMaterialComponent::updateTexture
//

static inline void updateTexture(OtTexture& texture, const std::filesystem::path& path, bool& flag) {
	if (flag) {
		if (std::filesystem::is_regular_file(path)) {
			texture.loadFromFile(path.string());

		} else {
			texture.clear();
		}

		flag = false;
	}
}

void OtMaterialComponent::update() {
	updateTexture(albedoTexture, albedoTexturePath, updateAlbedoTexture);
	updateTexture(normalTexture, normalTexturePath, updateNormalTexture);
	updateTexture(metallicTexture, metallicTexturePath, updateMetallicTexture);
	updateTexture(roughnessTexture, roughnessTexturePath, updateRoughnessTexture);
	updateTexture(emissiveTexture, emissiveTexturePath, updateEmissiveTexture);
	updateTexture(aoTexture, aoTexturePath, updateAoTexture);
}
