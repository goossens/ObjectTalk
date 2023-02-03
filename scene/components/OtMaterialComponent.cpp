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
	changed |= editPath("Albedo Texture", albedoTexturePath, updateAlbedoTexture);
	changed |= editPath("Normal Texture", normalTexturePath, updateNormalTexture);
	changed |= ImGui::SliderFloat("Metallic", &metallic, 0.0f, 1.0f, "%.2f");
	changed |= editPath("Metallic Texture", metallicTexturePath, updateMetallicTexture);
	changed |= ImGui::SliderFloat("Roughness", &roughness, 0.0f, 1.0f, "%.2f");
	changed |= editPath("Roughness Texture", roughnessTexturePath, updateRoughnessTexture);
	changed |= ImGui::SliderFloat("Ambient Occlusion", &ao, 0.0f, 1.0f, "%.2f");
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
	data["albedoTexture"] = OtComponentGetRelativePath(albedoTexturePath, basedir);
	data["normalTexture"] = OtComponentGetRelativePath(normalTexturePath, basedir);
	data["metallic"] = metallic;
	data["metallicTexture"] = OtComponentGetRelativePath(metallicTexturePath, basedir);
	data["roughness"] = roughness;
	data["roughnessTexture"] = OtComponentGetRelativePath(roughnessTexturePath, basedir);
	data["ao"] = ao;
	data["aoTexture"] = OtComponentGetRelativePath(aoTexturePath, basedir);
	return data;
}


//
//	OtMaterialComponent::deserialize
//

void OtMaterialComponent::deserialize(nlohmann::json data, std::filesystem::path* basedir) {
	albedo = data["albedo"];
	albedoTexturePath = OtComponentGetAbsolutePath(data, "albedoTexture", basedir);
	normalTexturePath = OtComponentGetAbsolutePath(data, "normalTexture", basedir);
	metallic = data["metallic"];
	metallicTexturePath = OtComponentGetAbsolutePath(data, "metallicTexture", basedir);
	roughness = data["roughness"];
	roughnessTexturePath = OtComponentGetAbsolutePath(data, "roughnessTexture", basedir);
	ao = data["ao"];
	aoTexturePath = OtComponentGetAbsolutePath(data, "aoTexture", basedir);

	updateAlbedoTexture = true;
	updateNormalTexture = true;
	updateMetallicTexture = true;
	updateRoughnessTexture = true;
	updateAoTexture = true;
}


//
//	OtMaterialComponent::isValid
//

static inline void updateTexture(OtTexture& texture, const std::filesystem::path& path, bool& flag) {
	if (flag) {
		if (std::filesystem::is_regular_file(path)) {
			texture.loadFromFile(path);

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
	updateTexture(aoTexture, aoTexturePath, updateAoTexture);
}
