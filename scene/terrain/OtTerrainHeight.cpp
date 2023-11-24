//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtUi.h"

#include "OtTerrainHeight.h"


//
//	OtTerrainHeight::renderGUI
//

bool OtTerrainHeight::renderGUI() {
	// function result
	bool changed = false;

	// render button + label
	if (ImGui::Button("Edit", ImVec2(ImGui::CalcItemWidth(), 0.0f))) {
		ImGui::OpenPopup("TerrainHeightPopup");
	}

	ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
	ImGui::TextUnformatted("Heightmap");

	// open popup (if required)
	if (ImGui::BeginPopup("TerrainHeightPopup")) {
		if (ImGui::BeginTable("layout", 2)) {
			ImGui::TableNextRow();

			ImGui::TableNextColumn();
			changed |= OtUiSelectorPowerOfTwo("Heightmap Size", heightmapSize, 256, 2048);
			changed |= noise.renderGUI();

			ImGui::TableNextColumn();
			update();
			ImGui::Image((void*)(intptr_t) heightmap.getColorTextureIndex(), ImVec2(512, 512));

			ImGui::EndTable();
		}

		ImGui::EndPopup();
	}

	dirty |= changed;
	return changed;
}


//
//	OtTerrainHeight::serialize
//

nlohmann::json OtTerrainHeight::serialize(std::filesystem::path* basedir) {
	auto data = nlohmann::json::object();
	data["size"] = heightmapSize;
	data["noise"] = noise.serialize(basedir);
	return data;
}


//
//	OtTerrainHeight::deserialize
//

void OtTerrainHeight::deserialize(nlohmann::json data, std::filesystem::path* basedir) {
	heightmapSize = data.value("size", 256);

	if (data.contains("noise")) {
		noise.deserialize(data["noise"], basedir);
	}

	dirty = true;
}


//
//	OtTerrainHeight::bindHeightmap
//

void OtTerrainHeight::bindHeightmap(OtSampler& sampler, int unit) {
	// update map (if required)
	update();

	// do the actual binding
	heightmap.bindColorTexture(sampler, unit);
}


//
//	OtTerrainHeight::update
//

void OtTerrainHeight::update() {
	// update size of framebuffer (if required)
	if (dirty) {
		heightmap.update(heightmapSize, heightmapSize);

		// create new noise map
		noise.render(heightmap);

		// reset the flag
		dirty = false;
	}
}
