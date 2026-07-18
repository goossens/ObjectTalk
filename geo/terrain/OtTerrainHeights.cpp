//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
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
	auto changed = false;

	// render button + label
	if (ImGui::Button("Edit##Heights", ImVec2(ImGui::CalcItemWidth(), 0.0f))) {
		ImGui::OpenPopup("TerrainHeightPopup");
	}

	ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
	OtUi::text("Heights");

	// open popup (if required)
	if (ImGui::BeginPopup("TerrainHeightPopup")) {
		if (ImGui::BeginTable("layout", 2)) {
			ImGui::TableNextRow();

			ImGui::TableNextColumn();
			changed |= OtUi::selectorPowerOfTwo("Heightmap Size", &heightMapSize, 256, 2048);

			changed |= OtUi::dragInt("Frequency", &noise.frequency, 2, 20);
			changed |= OtUi::dragInt("Lacunarity", &noise.lacunarity, 1, 10);
			changed |= OtUi::dragFloat("Amplitude", &noise.amplitude, 0.1f, 1.0f);
			changed |= OtUi::dragFloat("Persistence", &noise.persistence, 0.1f, 1.0f);
			changed |= OtUi::dragInt("Octaves", &noise.octaves, 1, 10);

			changed |= OtUi::dragFloat("Normal Strength", &normals.normalStrength, 1.0f, 100.0f);

			ImGui::TableNextColumn();

			if (normalmap.isValid()) {
				ImGui::Image(normalmap.getTextureID(), ImVec2(512.0f, 512.0f));
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

nlohmann::json OtTerrainHeights::serialize([[maybe_unused]] std::string* basedir) {
	auto data = nlohmann::json::object();
	data["size"] = heightMapSize;

	data["frequency"] = noise.frequency;
	data["lacunarity"] = noise.lacunarity;
	data["amplitude"] = noise.amplitude;
	data["persistence"] = noise.persistence;
	data["octaves"] = noise.octaves;

	data["normalStrength"] = normals.normalStrength;

	return data;
}


//
//	OtTerrainHeights::deserialize
//

void OtTerrainHeights::deserialize(nlohmann::json& data, [[maybe_unused]] std::string* basedir) {
	heightMapSize = data.value("size", 256);

	noise.frequency = data.value("frequency", 10);
	noise.lacunarity = data.value("lacunarity", 2);
	noise.amplitude = data.value("amplitude", 0.5f);
	noise.persistence = data.value("persistence", 0.5f);
	noise.octaves = data.value("octaves", 5);

	normals.normalStrength = data.value("normalStrength", 10.0f);

	dirty = true;
}


//
//	OtTerrainHeights::update
//

void OtTerrainHeights::update() {
	// update size of framebuffer
	heightmap.update(heightMapSize, heightMapSize, OtTexture::Format::r32, OtTexture::Usage::rwDefault);
	normalmap.update(heightMapSize, heightMapSize, OtTexture::Format::rgba32, OtTexture::Usage::rwDefault);

	// create noise map
	noise.render(heightmap);

	// create normal map
	normals.render(heightmap, normalmap);

	// reset flag
	dirty = false;
}
