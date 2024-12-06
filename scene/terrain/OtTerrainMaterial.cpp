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

#include "OtTerrainMaterial.h"


//
//	OtTerrainMaterial::renderUI
//

#define W1() ImGui::SetNextItemWidth(200.0f)
#define W2() ImGui::SetNextItemWidth(100.0f)

bool OtTerrainMaterial::renderUI() {
	bool changed = false;

	// render button + label
	if (ImGui::Button("Edit##Material", ImVec2(ImGui::CalcItemWidth(), 0.0f))) {
		ImGui::OpenPopup("TerrainMaterialPopup");
	}

	ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
	ImGui::TextUnformatted("Material");

	// open popup (if required)
	if (ImGui::BeginPopup("TerrainMaterialPopup")) {
		if (ImGui::BeginTable("layout", 6)) {
			ImGui::TableNextRow();
			ImGui::TableNextColumn(); ImGui::TextUnformatted("Color");
			ImGui::TableNextColumn(); ImGui::TextUnformatted("Texture");
			ImGui::TableNextColumn(); ImGui::TextUnformatted("Scale");
			ImGui::TableNextColumn(); ImGui::TextUnformatted("Transition");
			ImGui::TableNextColumn(); ImGui::TextUnformatted("Overlap");

			ImGui::TableNextRow();
			ImGui::TableNextColumn(); W1(); changed |= ImGui::ColorEdit3("##region1Color", glm::value_ptr(region1Color));
			ImGui::TableNextColumn(); W1(); changed |= region1Texture.renderUI("##region1Texture");
			ImGui::TableNextColumn(); W2(); changed |= OtUi::dragFloat("##region1TextureScale", &region1TextureScale, 1.0f, 500.0f);
			ImGui::TableNextColumn(); W2(); changed |= OtUi::dragFloat("##region1Transition", &region1Transition, 0.0f, 1.0f);
			ImGui::TableNextColumn(); W2(); changed |= OtUi::dragFloat("##region1Overlap", &region1Overlap, 0.0f, 1.0f);
			ImGui::TableNextColumn(); ImGui::TextUnformatted("Region 1");

			ImGui::TableNextRow();
			ImGui::TableNextColumn(); W1(); changed |= ImGui::ColorEdit3("##region2Color", glm::value_ptr(region2Color));
			ImGui::TableNextColumn(); W1(); changed |= region2Texture.renderUI("##region2Texture");
			ImGui::TableNextColumn(); W2(); changed |= OtUi::dragFloat("##region2TextureScale", &region2TextureScale, 1.0f, 500.0f);
			ImGui::TableNextColumn(); W2(); changed |= OtUi::dragFloat("##region2Transition", &region2Transition, 0.0f, 1.0f);
			ImGui::TableNextColumn(); W2(); changed |= OtUi::dragFloat("##region2Overlap", &region2Overlap, 0.0f, 1.0f);
			ImGui::TableNextColumn(); ImGui::TextUnformatted("Region 2");

			ImGui::TableNextRow();
			ImGui::TableNextColumn(); W1(); changed |= ImGui::ColorEdit3("##region3Color", glm::value_ptr(region3Color));
			ImGui::TableNextColumn(); W1(); changed |= region3Texture.renderUI("##region3Texture");
			ImGui::TableNextColumn(); W2(); changed |= OtUi::dragFloat("##region3TextureScale", &region3TextureScale, 1.0f, 500.0f);
			ImGui::TableNextColumn(); W2(); changed |= OtUi::dragFloat("##region3Transition", &region3Transition, 0.0f, 1.0f);
			ImGui::TableNextColumn(); W2(); changed |= OtUi::dragFloat("##regio3Overlap", &region3Overlap, 0.0f, 1.0f);
			ImGui::TableNextColumn(); ImGui::TextUnformatted("Region 3");

			ImGui::TableNextRow();
			ImGui::TableNextColumn(); W1(); changed |= ImGui::ColorEdit3("##region4Color", glm::value_ptr(region4Color));
			ImGui::TableNextColumn(); W1(); changed |= region4Texture.renderUI("##region4Texture");
			ImGui::TableNextColumn(); W2(); changed |= OtUi::dragFloat("##region4TextureScale", &region4TextureScale, 1.0f, 500.0f);
			ImGui::TableNextColumn();
			ImGui::TableNextColumn();
			ImGui::TableNextColumn(); ImGui::TextUnformatted("Region 4");
			ImGui::EndTable();
		}

		ImGui::EndPopup();
	}

	return changed;
}


//
//	OtTerrainMaterial::serialize
//

nlohmann::json OtTerrainMaterial::serialize(std::string* basedir) {
	auto data = nlohmann::json::object();
	data["region1Color"] = region1Color;
	data["region2Color"] = region2Color;
	data["region3Color"] = region3Color;
	data["region4Color"] = region4Color;

	data["region1Texture"] = OtAssetSerialize(region1Texture.getPath(), basedir);
	data["region2Texture"] = OtAssetSerialize(region2Texture.getPath(), basedir);
	data["region3Texture"] = OtAssetSerialize(region3Texture.getPath(), basedir);
	data["region4Texture"] = OtAssetSerialize(region4Texture.getPath(), basedir);

	data["region1TextureScale"] = region1TextureScale;
	data["region2TextureScale"] = region2TextureScale;
	data["region3TextureScale"] = region3TextureScale;
	data["region4TextureScale"] = region4TextureScale;

	data["region1Transition"] = region1Transition;
	data["region2Transition"] = region2Transition;
	data["region3Transition"] = region3Transition;

	data["region1Overlap"] = region1Overlap;
	data["region2Overlap"] = region2Overlap;
	data["region3Overlap"] = region3Overlap;

	return data;
}


//
//	OtTerrainMaterial::deserialize
//

void OtTerrainMaterial::deserialize(nlohmann::json data, std::string* basedir) {
	region1Color = data.value("region1Color", glm::vec3(0.965f, 0.894f, 0.678f));
	region2Color = data.value("region2Color", glm::vec3(0.494f, 0.784f, 0.314f));
	region3Color = data.value("region3Color", glm::vec3(0.584f, 0.553f, 0.522f));
	region4Color = data.value("region4Color", glm::vec3(0.9f, 0.9f, 0.9f));

	region1Texture = OtAssetDeserialize(&data, "region1Texture", basedir);
	region2Texture = OtAssetDeserialize(&data, "region2Texture", basedir);
	region3Texture = OtAssetDeserialize(&data, "region3Texture", basedir);
	region4Texture = OtAssetDeserialize(&data, "region4Texture", basedir);

	region1TextureScale = data.value("region1TextureScale", 1.0f);
	region2TextureScale = data.value("region2TextureScale", 1.0f);
	region3TextureScale = data.value("region3TextureScale", 1.0f);
	region4TextureScale = data.value("region4TextureScale", 1.0f);

	region1Transition = data.value("region1Transition", 0.75f);
	region2Transition = data.value("region2Transition", 0.25f);
	region3Transition = data.value("region3Transition", 0.5f);

	region1Overlap = data.value("region1Overlap", 0.01f);
	region2Overlap = data.value("region2Overlap", 0.01f);
	region3Overlap = data.value("region3Overlap", 0.01f);
}
