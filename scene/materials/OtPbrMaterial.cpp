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

#include "OtPbrMaterial.h"
#include "OtPathTools.h"


//
//	OtPbrMaterialClass::update
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

void OtPbrMaterialClass::update() {
	updateTexture(albedoTexture, albedoTexturePath, updateAlbedoTexture);
	updateTexture(normalTexture, normalTexturePath, updateNormalTexture);
	updateTexture(metallicTexture, metallicTexturePath, updateMetallicTexture);
	updateTexture(roughnessTexture, roughnessTexturePath, updateRoughnessTexture);
	updateTexture(emissiveTexture, emissiveTexturePath, updateEmissiveTexture);
	updateTexture(aoTexture, aoTexturePath, updateAoTexture);
}


//
//	OtPbrMaterialClass::renderGUI
//

#define W() ImGui::SetNextItemWidth(250.0f)

bool OtPbrMaterialClass::renderGUI() {
	bool changed = false;
	float size = ImGui::GetFrameHeight();

	// render button
	if (ImGui::Button("e", ImVec2(size, size))) {
		ImGui::OpenPopup("PbrPopup");
	}

	// open popup (if required)
	if (ImGui::BeginPopup("PbrPopup")) {
		if (ImGui::BeginTable("layout", 3)) {
			ImGui::TableNextRow();
			ImGui::TableNextColumn(); ImGui::TextUnformatted("Fixed Values");
			ImGui::TableNextColumn(); ImGui::TextUnformatted("Texure Maps");

			ImGui::TableNextRow();
			ImGui::TableNextColumn(); W(); changed |= ImGui::ColorEdit4("##albedoColor", glm::value_ptr(albedo));
			ImGui::TableNextColumn(); W(); changed |= OtPathEdit("##albedoTexture", albedoTexturePath, updateAlbedoTexture);
			ImGui::TableNextColumn(); ImGui::TextUnformatted("Albedo");

			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::TableNextColumn(); W(); changed |= OtPathEdit("##normalTexture", normalTexturePath, updateNormalTexture);
			ImGui::TableNextColumn(); ImGui::TextUnformatted("Normals");

			ImGui::TableNextRow();
			ImGui::TableNextColumn(); W(); changed |= ImGui::SliderFloat("##metallic", &metallic, 0.0f, 1.0f, "%.2f");
			ImGui::TableNextColumn(); W(); changed |= OtPathEdit("##metallicTexture", metallicTexturePath, updateMetallicTexture);
			ImGui::TableNextColumn(); ImGui::TextUnformatted("Metallic");

			ImGui::TableNextRow();
			ImGui::TableNextColumn(); W(); ImGui::SliderFloat("##roughness", &roughness, 0.0f, 1.0f, "%.2f");
			ImGui::TableNextColumn(); W(); changed |= OtPathEdit("##roughnessTexture", roughnessTexturePath, updateRoughnessTexture);
			ImGui::TableNextColumn(); ImGui::TextUnformatted("Roughness");

			ImGui::TableNextRow();
			ImGui::TableNextColumn(); W(); changed |= ImGui::SliderFloat("##emissive", &emissive, 0.0f, 4.0f, "%.2f");
			ImGui::TableNextColumn(); W(); changed |= OtPathEdit("##emissiveTexture", emissiveTexturePath, updateEmissiveTexture);
			ImGui::TableNextColumn(); ImGui::TextUnformatted("Emissive");

			ImGui::TableNextRow();
			ImGui::TableNextColumn(); W(); changed |= ImGui::SliderFloat("##ambientOcclusion", &ao, 0.0f, 1.0f, "%.2f");
			ImGui::TableNextColumn(); W(); changed |= OtPathEdit("##aoTexture", aoTexturePath, updateAoTexture);
			ImGui::TableNextColumn(); ImGui::TextUnformatted("Ambient Occlusion");
			ImGui::EndTable();
		}

		ImGui::EndPopup();
	}

	if (changed) {
		update();
	}

	return changed;
}


//
//	OtPbrMaterialClass::serialize
//

nlohmann::json OtPbrMaterialClass::serialize(std::filesystem::path* basedir) {
	auto data = nlohmann::json::object();
	data["type"] = name;
	data.update(OtMaterialClass::serialize(basedir));

	data["albedo"] = albedo;
	data["metallic"] = metallic;
	data["roughness"] = roughness;
	data["emissive"] = emissive;
	data["ao"] = ao;

	data["albedoTexture"] = OtPathGetRelative(albedoTexturePath, basedir);
	data["normalTexture"] = OtPathGetRelative(normalTexturePath, basedir);
	data["metallicTexture"] = OtPathGetRelative(metallicTexturePath, basedir);
	data["roughnessTexture"] = OtPathGetRelative(roughnessTexturePath, basedir);
	data["emissiveTexture"] = OtPathGetRelative(emissiveTexturePath, basedir);
	data["aoTexture"] = OtPathGetRelative(aoTexturePath, basedir);
	return data;
}


//
//	OtPbrMaterialClass::deserialize
//

void OtPbrMaterialClass::deserialize(nlohmann::json data, std::filesystem::path* basedir) {
	albedo = data.value("albedo", glm::vec4(1.0f));
	metallic = data.value("metallic", 0.5f);
	roughness = data.value("roughness", 0.5f);
	emissive = data.value("emissive", 0.0f);
	ao = data.value("ao", 1.0f);

	albedoTexturePath = OtPathGetAbsolute(data, "albedoTexture", basedir);
	normalTexturePath = OtPathGetAbsolute(data, "normalTexture", basedir);
	metallicTexturePath = OtPathGetAbsolute(data, "metallicTexture", basedir);
	roughnessTexturePath = OtPathGetAbsolute(data, "roughnessTexture", basedir);
	emissiveTexturePath = OtPathGetAbsolute(data, "emissiveTexture", basedir);
	aoTexturePath = OtPathGetAbsolute(data, "aoTexture", basedir);

	updateAlbedoTexture = !albedoTexturePath.empty();
	updateNormalTexture = !normalTexturePath.empty();
	updateMetallicTexture = !metallicTexturePath.empty();
	updateRoughnessTexture = !roughnessTexturePath.empty();
	updateEmissiveTexture = !emissiveTexturePath.empty();
	updateAoTexture = !aoTexturePath.empty();
}


//
//	OtPbrMaterialClass::getMeta
//

OtType OtPbrMaterialClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtPbrMaterialClass>("PbrMaterial", OtMaterialClass::getMeta());
	}

	return type;
}
