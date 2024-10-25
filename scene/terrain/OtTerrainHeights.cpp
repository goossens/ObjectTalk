//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtUi.h"

#include "OtTerrainHeights.h"


//
//	OtTerrainHeights::renderUI
//

bool OtTerrainHeights::renderUI() {
	// function result
	bool changed = false;

	// render button + label
	if (ImGui::Button("Edit##Heights", ImVec2(ImGui::CalcItemWidth(), 0.0f))) {
		ImGui::OpenPopup("TerrainHeightPopup");
	}

	ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
	ImGui::TextUnformatted("Heights");

	// open popup (if required)
	if (ImGui::BeginPopup("TerrainHeightPopup")) {
		if (ImGui::BeginTable("layout", 2)) {
			ImGui::TableNextRow();

			ImGui::TableNextColumn();
			changed |= OtUiSelectorPowerOfTwo("Heightmap Size", &heightmapSize, 256, 2048);
			changed |= OtUiDragFloat("Normal Strength", &normalStrength, 1.0f, 100.0f);
			changed |= OtUiDragInt("Frequency", &frequency, 2, 20);
			changed |= OtUiDragInt("Lacunarity", &lacunarity, 1, 10);
			changed |= OtUiDragFloat("Amplitude", &amplitude, 0.1f, 1.0f);
			changed |= OtUiDragFloat("Persistence", &persistence, 0.1f, 1.0f);
			changed |= OtUiDragInt("Octaves", &octaves, 1, 10);

			ImGui::TableNextColumn();

			if (normalmap.isValid()) {
				ImGui::Image((ImTextureID)(intptr_t) normalmap.getColorTextureIndex(), ImVec2(512, 512));
			}

			ImGui::EndTable();
		}

		ImGui::EndPopup();
	}

	dirty |= changed;
	return changed;
}


//
//	OtTerrainHeights::serialize
//

nlohmann::json OtTerrainHeights::serialize(std::string* basedir) {
	auto data = nlohmann::json::object();
	data["size"] = heightmapSize;
	data["normalStrength"] = normalStrength;
	data["frequency"] = frequency;
	data["lacunarity"] = lacunarity;
	data["amplitude"] = amplitude;
	data["persistence"] = persistence;
	data["octaves"] = octaves;
	return data;
}


//
//	OtTerrainHeights::deserialize
//

void OtTerrainHeights::deserialize(nlohmann::json data, std::string* basedir) {
	heightmapSize = data.value("size", 256);
	normalStrength = data.value("normalStrength", 10.0f);
	frequency = data.value("frequency", 10);
	lacunarity = data.value("lacunarity", 2);
	amplitude = data.value("amplitude", 0.5f);
	octaves = data.value("octaves", 5);
	dirty = true;
}


//
//	OtTerrainHeights::update
//

void OtTerrainHeights::update(OtTileableFbm& noise, OtNormalMapper& normals) {
	// update size of framebuffer
	heightmap.update(heightmapSize, heightmapSize);
	normalmap.update(heightmapSize, heightmapSize);

	// create noise map
	noise.setFrequency(frequency);
	noise.setLacunarity(lacunarity);
	noise.setAmplitude(amplitude);
	noise.setPersistence(persistence);
	noise.setOctaves(octaves);
	noise.render(heightmap);

	// create normalmap
	normals.setStrength(normalStrength);
	normals.includeHeight(true);
	normals.render(heightmap, normalmap);

	// reset flag
	dirty = false;
}
