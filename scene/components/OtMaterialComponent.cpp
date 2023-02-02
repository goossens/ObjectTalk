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
//	OtMaterialComponent::editPath
//

bool OtMaterialComponent::editPath(const char* label, std::filesystem::path& path, OtTexture& texture) {
	bool changed = false;

	if (OtUiFileSelector(label, path)) {
		texture.clear();

		if (std::filesystem::is_regular_file(path)) {
			update = true;
		}

		changed = true;
	}

	return changed;
}


//
//	OtMaterialComponent::renderGUI
//

bool OtMaterialComponent::renderGUI() {
	bool changed = false;
	changed |= ImGui::ColorEdit3("Albedo Color", glm::value_ptr(albedo));
	changed |= editPath("Albedo Texture", albedoTexturePath, albedoTexture);
	changed |= editPath("Normal Texture", normalTexturePath, normalTexture);
	changed |= ImGui::SliderFloat("Metalic", &metalic, 0.0f, 1.0f, "%.2f");
	changed |= editPath("Metalic Texture", metalicTexturePath, metalicTexture);
	changed |= ImGui::SliderFloat("Roughness", &roughness, 0.0f, 1.0f, "%.2f");
	changed |= editPath("Roughness Texture", roughnessTexturePath, roughnessTexture);
	changed |= ImGui::SliderFloat("Ambient Occlusion", &ao, 0.0f, 1.0f, "%.2f");
	changed |= editPath("AO Texture", aoTexturePath, aoTexture);
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
	data["metalic"] = metalic;
	data["metalicTexture"] = OtComponentGetRelativePath(metalicTexturePath, basedir);
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
	metalic = data["metalic"];
	metalicTexturePath = OtComponentGetAbsolutePath(data, "metalicTexture", basedir);
	roughness = data["roughness"];
	roughnessTexturePath = OtComponentGetAbsolutePath(data, "roughnessTexture", basedir);
	ao = data["ao"];
	aoTexturePath = OtComponentGetAbsolutePath(data, "aoTexture", basedir);
}
