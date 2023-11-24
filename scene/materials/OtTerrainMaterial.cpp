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
//	OtTerrainMaterialClass::renderGUI
//

#define W() ImGui::SetNextItemWidth(200.0f)

bool OtTerrainMaterialClass::renderGUI() {
	bool changed = false;
	float size = ImGui::GetFrameHeight();

	// render button
	if (ImGui::Button("e", ImVec2(size, size))) {
		ImGui::OpenPopup("TerrainMaterialPopup");
	}

	// open popup (if required)
	if (ImGui::BeginPopup("TerrainMaterialPopup")) {
		if (ImGui::BeginTable("layout", 5)) {
			ImGui::TableNextRow();
			ImGui::TableNextColumn(); ImGui::TextUnformatted("Color");
			ImGui::TableNextColumn(); ImGui::TextUnformatted("Texture");
			ImGui::TableNextColumn(); ImGui::TextUnformatted("Transition");
			ImGui::TableNextColumn(); ImGui::TextUnformatted("Overlap");

			ImGui::TableNextRow();
			ImGui::TableNextColumn(); W(); changed |= ImGui::ColorEdit3("##region1Color", glm::value_ptr(region1Color));
			ImGui::TableNextColumn(); W(); changed |= region1Texture.renderGUI("##region1Texture");
			ImGui::TableNextColumn(); W(); changed |= ImGui::DragFloat("##region1Transition", &region1Transition, 0.01f, 0.0f, 1.0f);
			ImGui::TableNextColumn(); W(); changed |= ImGui::DragFloat("##region1Overlap", &region1Overlap, 0.01f, 0.0f, 1.0f);
			ImGui::TableNextColumn(); ImGui::TextUnformatted("Region 1");

			ImGui::TableNextRow();
			ImGui::TableNextColumn(); W(); changed |= ImGui::ColorEdit3("##region2Color", glm::value_ptr(region2Color));
			ImGui::TableNextColumn(); W(); changed |= region2Texture.renderGUI("##region2Texture");
			ImGui::TableNextColumn(); W(); changed |= ImGui::DragFloat("##region2Transition", &region2Transition, 0.01f, 0.0f, 1.0f);
			ImGui::TableNextColumn(); W(); changed |= ImGui::DragFloat("##region2Overlap", &region2Overlap, 0.01f, 0.0f, 1.0f);
			ImGui::TableNextColumn(); ImGui::TextUnformatted("Region 2");

			ImGui::TableNextRow();
			ImGui::TableNextColumn(); W(); changed |= ImGui::ColorEdit3("##region3Color", glm::value_ptr(region3Color));
			ImGui::TableNextColumn(); W(); changed |= region3Texture.renderGUI("##region3Texture");
			ImGui::TableNextColumn(); W(); changed |= ImGui::DragFloat("##region3Transition", &region3Transition, 0.01f, 0.0f, 1.0f);
			ImGui::TableNextColumn(); W(); changed |= ImGui::DragFloat("##regio3Overlap", &region3Overlap, 0.01f, 0.0f, 1.0f);
			ImGui::TableNextColumn(); ImGui::TextUnformatted("Region 3");

			ImGui::TableNextRow();
			ImGui::TableNextColumn(); W(); changed |= ImGui::ColorEdit3("##region4Color", glm::value_ptr(region4Color));
			ImGui::TableNextColumn(); W(); changed |= region4Texture.renderGUI("##region4Texture");
			ImGui::TableNextColumn();
			ImGui::TableNextColumn();
			ImGui::TableNextColumn(); ImGui::TextUnformatted("Region 4");

			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::TableNextColumn(); W(); changed |= ImGui::SliderFloat("##scale", &scale, 0.0f, 100.0f, "%.1f");
			ImGui::TableNextColumn();
			ImGui::TableNextColumn();
			ImGui::TableNextColumn(); ImGui::TextUnformatted("Scale");
			ImGui::EndTable();
		}

		ImGui::EndPopup();
	}

	return changed;
}


//
//	OtTerrainMaterialClass::serialize
//

nlohmann::json OtTerrainMaterialClass::serialize(std::filesystem::path* basedir) {
	auto data = nlohmann::json::object();
	data["type"] = name;
	data.update(OtMaterialClass::serialize(basedir));

	data["region1Color"] = region1Color;
	data["region2Color"] = region2Color;
	data["region3Color"] = region3Color;
	data["region4Color"] = region4Color;

	data["region1Texture"] = OtPathGetRelative(region1Texture.getPath(), basedir);
	data["region2Texture"] = OtPathGetRelative(region2Texture.getPath(), basedir);
	data["region3Texture"] = OtPathGetRelative(region3Texture.getPath(), basedir);
	data["region4Texture"] = OtPathGetRelative(region4Texture.getPath(), basedir);

	data["region1Transition"] = region1Transition;
	data["region2Transition"] = region2Transition;
	data["region3Transition"] = region3Transition;

	data["region1Overlap"] = region1Overlap;
	data["region2Overlap"] = region2Overlap;
	data["region3Overlap"] = region3Overlap;

	data["scale"] = scale;

	return data;
}


//
//	OtTerrainMaterialClass::deserialize
//

void OtTerrainMaterialClass::deserialize(nlohmann::json data, std::filesystem::path* basedir) {
	OtMaterialClass::deserialize(data, basedir);

	region1Color = data.value("region1Color", glm::vec3(0.965f, 0.894f, 0.678f));
	region2Color = data.value("region2Color", glm::vec3(0.494f, 0.784f, 0.314f));
	region3Color = data.value("region3Color", glm::vec3(0.584f, 0.553f, 0.522f));
	region4Color = data.value("region4Color", glm::vec3(0.9f, 0.9f, 0.9f));

	region1Texture = OtPathGetAbsolute(data, "region1Texture", basedir);
	region2Texture = OtPathGetAbsolute(data, "region2Texture", basedir);
	region3Texture = OtPathGetAbsolute(data, "region3Texture", basedir);
	region4Texture = OtPathGetAbsolute(data, "region4Texture", basedir);

	region1Transition = data.value("region1Transition", 0.75f);
	region2Transition = data.value("region2Transition", 0.25f);
	region3Transition = data.value("region3Transition", 0.5f);

	region1Overlap = data.value("region1Overlap", 0.01f);
	region2Overlap = data.value("region2Overlap", 0.01f);
	region3Overlap = data.value("region3Overlap", 0.01f);

	scale = data.value("scale", 1.0f);
}


//
//	OtTerrainMaterialClass::getMeta
//

OtType OtTerrainMaterialClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtTerrainMaterialClass>("TerrainMaterial", OtMaterialClass::getMeta());
	}

	return type;
}
