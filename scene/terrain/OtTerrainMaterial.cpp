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
#include "OtTerrainMaterial.h"


//
//	OtTerrainMaterial::renderGUI
//

#define W() ImGui::SetNextItemWidth(200.0f)

bool OtTerrainMaterial::renderGUI() {
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
			ImGui::TableNextColumn(); ImGui::TextUnformatted("Texture Scale");
			ImGui::TableNextColumn(); ImGui::TextUnformatted("Transition");
			ImGui::TableNextColumn(); ImGui::TextUnformatted("Overlap");

			ImGui::TableNextRow();
			ImGui::TableNextColumn(); W(); changed |= ImGui::ColorEdit3("##region1Color", glm::value_ptr(region1Color));
			ImGui::TableNextColumn(); W(); changed |= region1Texture.renderGUI("##region1Texture");
			ImGui::TableNextColumn(); W(); changed |= ImGui::DragFloat("##region1TextureScale", &region1TextureScale, 0.1f, 1.0f, 100.0f);
			ImGui::TableNextColumn(); W(); changed |= ImGui::DragFloat("##region1Transition", &region1Transition, 0.01f, 0.0f, 1.0f);
			ImGui::TableNextColumn(); W(); changed |= ImGui::DragFloat("##region1Overlap", &region1Overlap, 0.01f, 0.0f, 1.0f);
			ImGui::TableNextColumn(); ImGui::TextUnformatted("Region 1");

			ImGui::TableNextRow();
			ImGui::TableNextColumn(); W(); changed |= ImGui::ColorEdit3("##region2Color", glm::value_ptr(region2Color));
			ImGui::TableNextColumn(); W(); changed |= region2Texture.renderGUI("##region2Texture");
			ImGui::TableNextColumn(); W(); changed |= ImGui::DragFloat("##region2TextureScale", &region2TextureScale, 0.1f, 1.0f, 100.0f);
			ImGui::TableNextColumn(); W(); changed |= ImGui::DragFloat("##region2Transition", &region2Transition, 0.01f, 0.0f, 1.0f);
			ImGui::TableNextColumn(); W(); changed |= ImGui::DragFloat("##region2Overlap", &region2Overlap, 0.01f, 0.0f, 1.0f);
			ImGui::TableNextColumn(); ImGui::TextUnformatted("Region 2");

			ImGui::TableNextRow();
			ImGui::TableNextColumn(); W(); changed |= ImGui::ColorEdit3("##region3Color", glm::value_ptr(region3Color));
			ImGui::TableNextColumn(); W(); changed |= region3Texture.renderGUI("##region3Texture");
			ImGui::TableNextColumn(); W(); changed |= ImGui::DragFloat("##region3TextureScale", &region3TextureScale, 0.1f, 1.0f, 100.0f);
			ImGui::TableNextColumn(); W(); changed |= ImGui::DragFloat("##region3Transition", &region3Transition, 0.01f, 0.0f, 1.0f);
			ImGui::TableNextColumn(); W(); changed |= ImGui::DragFloat("##regio3Overlap", &region3Overlap, 0.01f, 0.0f, 1.0f);
			ImGui::TableNextColumn(); ImGui::TextUnformatted("Region 3");

			ImGui::TableNextRow();
			ImGui::TableNextColumn(); W(); changed |= ImGui::ColorEdit3("##region4Color", glm::value_ptr(region4Color));
			ImGui::TableNextColumn(); W(); changed |= region4Texture.renderGUI("##region4Texture");
			ImGui::TableNextColumn(); W(); changed |= ImGui::DragFloat("##region4TextureScale", &region4TextureScale, 0.1f, 1.0f, 100.0f);
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

nlohmann::json OtTerrainMaterial::serialize(std::filesystem::path* basedir) {
	auto data = nlohmann::json::object();
	data["region1Color"] = region1Color;
	data["region2Color"] = region2Color;
	data["region3Color"] = region3Color;
	data["region4Color"] = region4Color;

	data["region1Texture"] = OtPathGetRelative(region1Texture.getPath(), basedir);
	data["region2Texture"] = OtPathGetRelative(region2Texture.getPath(), basedir);
	data["region3Texture"] = OtPathGetRelative(region3Texture.getPath(), basedir);
	data["region4Texture"] = OtPathGetRelative(region4Texture.getPath(), basedir);

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

void OtTerrainMaterial::deserialize(nlohmann::json data, std::filesystem::path* basedir) {
	region1Color = data.value("region1Color", glm::vec3(0.965f, 0.894f, 0.678f));
	region2Color = data.value("region2Color", glm::vec3(0.494f, 0.784f, 0.314f));
	region3Color = data.value("region3Color", glm::vec3(0.584f, 0.553f, 0.522f));
	region4Color = data.value("region4Color", glm::vec3(0.9f, 0.9f, 0.9f));

	region1Texture = OtPathGetAbsolute(data, "region1Texture", basedir);
	region2Texture = OtPathGetAbsolute(data, "region2Texture", basedir);
	region3Texture = OtPathGetAbsolute(data, "region3Texture", basedir);
	region4Texture = OtPathGetAbsolute(data, "region4Texture", basedir);

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
