//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtReal.h"

#include "OtGlm.h"

#include "OtMaterial.h"


//
//	OtMaterial::renderUI
//

#define W() ImGui::SetNextItemWidth(250.0f)

bool OtMaterial::renderUI() {
	bool changed = false;

	// render button
	if (ImGui::Button("Edit", ImVec2(ImGui::CalcItemWidth(), 0.0f))) {
		ImGui::OpenPopup("MaterialPopup");
	}

	ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
	ImGui::TextUnformatted("Material");

	// open popup (if required)
	if (ImGui::BeginPopup("MaterialPopup")) {
		if (ImGui::BeginTable("layout", 3)) {
			ImGui::TableNextRow();
			ImGui::TableNextColumn(); ImGui::TextUnformatted("Fixed Values");
			ImGui::TableNextColumn(); ImGui::TextUnformatted("Texture Maps");

			ImGui::TableNextRow();
			ImGui::TableNextColumn(); W(); changed |= ImGui::ColorEdit4("##albedoColor", glm::value_ptr(albedo));
			ImGui::TableNextColumn(); W(); changed |= albedoTexture.renderUI("##albedoTexture");
			ImGui::TableNextColumn(); ImGui::TextUnformatted("Albedo");

			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::TableNextColumn(); W(); changed |= normalTexture.renderUI("##normalTexture");
			ImGui::TableNextColumn(); ImGui::TextUnformatted("Normals");

			ImGui::TableNextRow();
			ImGui::TableNextColumn(); W(); changed |= OtUi::dragFloat("##metallic", &metallic, 0.0f, 1.0f);
			ImGui::TableNextColumn(); W(); changed |= metallicRoughnessTexture.renderUI("##metallicRoughnessTexture");
			ImGui::TableNextColumn(); ImGui::TextUnformatted("Metallic");

			ImGui::TableNextRow();
			ImGui::TableNextColumn(); W(); OtUi::dragFloat("##roughness", &roughness, 0.0f, 1.0f);
			ImGui::TableNextColumn();
			ImGui::TableNextColumn(); ImGui::TextUnformatted("Roughness");

			ImGui::TableNextRow();
			ImGui::TableNextColumn(); W(); changed |= ImGui::ColorEdit3("##emissive", glm::value_ptr(emissive));
			ImGui::TableNextColumn(); W(); changed |= emissiveTexture.renderUI("##emissiveTexture");
			ImGui::TableNextColumn(); ImGui::TextUnformatted("Emissive");

			ImGui::TableNextRow();
			ImGui::TableNextColumn(); W(); changed |= OtUi::dragFloat("##ambientOcclusion", &ao, 0.0f, 1.0f);
			ImGui::TableNextColumn(); W(); changed |= aoTexture.renderUI("##aoTexture");
			ImGui::TableNextColumn(); ImGui::TextUnformatted("Ambient Occlusion");

			ImGui::TableNextRow();
			ImGui::TableNextColumn(); W(); changed |= OtUi::dragFloat("##scale", &scale, 0.0f, 100.0f);
			ImGui::TableNextColumn();
			ImGui::TableNextColumn(); ImGui::TextUnformatted("Scale");
			ImGui::EndTable();
		}

		ImGui::EndPopup();
	}

	return changed;
}


//
//	OtMaterial::serialize
//

nlohmann::json OtMaterial::serialize(std::string* basedir) {
	auto data = nlohmann::json::object();

	data["albedo"] = albedo;
	data["metallic"] = metallic;
	data["roughness"] = roughness;
	data["emissive"] = emissive;
	data["ao"] = ao;
	data["scale"] = scale;

	data["albedoTexture"] = OtAssetSerialize(albedoTexture.getPath(), basedir);
	data["normalTexture"] = OtAssetSerialize(normalTexture.getPath(), basedir);
	data["metallicRoughnessTexture"] = OtAssetSerialize(metallicRoughnessTexture.getPath(), basedir);
	data["emissiveTexture"] = OtAssetSerialize(emissiveTexture.getPath(), basedir);
	data["aoTexture"] = OtAssetSerialize(aoTexture.getPath(), basedir);
	return data;
}


//
//	OtMaterial::deserialize
//

void OtMaterial::deserialize(nlohmann::json data, std::string* basedir) {
	albedo = data.value("albedo", glm::vec4(1.0f));
	metallic = data.value("metallic", 0.5f);
	roughness = data.value("roughness", 0.5f);
	emissive = data.value("emissive", glm::vec3(0.0f));
	ao = data.value("ao", 1.0f);
	scale = data.value("scale", 1.0f);

	albedoTexture = OtAssetDeserialize(&data, "albedoTexture", basedir);
	normalTexture = OtAssetDeserialize(&data, "normalTexture", basedir);
	metallicRoughnessTexture = OtAssetDeserialize(&data, "metallicRoughnessTexture", basedir);
	emissiveTexture = OtAssetDeserialize(&data, "emissiveTexture", basedir);
	aoTexture = OtAssetDeserialize(&data, "aoTexture", basedir);
}
