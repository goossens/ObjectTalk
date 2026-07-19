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

#include "OtGlm.h"
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
		bool textureChanged = false;
		bool noiseChanged = false;
		bool erosionChanged = false;
		bool normalsChanged = false;

		if (ImGui::BeginTable("layout", 2)) {
			ImGui::TableNextRow();

			ImGui::TableNextColumn();
			ImGui::PushItemWidth(OtUi::size(6.0f));

			OtUi::header("Dimension");
			textureChanged |= OtUi::selectorPowerOfTwo("Size", &heightMapSize, 256, 2048);
			heightMapDensity = static_cast<float>(heightMapSize) / 256.0f;

			OtUi::header("Noise");
			noiseChanged |= OtUi::dragInt("Frequency", &noise.frequency, 2, 20);
			noiseChanged |= OtUi::dragInt("Lacunarity", &noise.lacunarity, 1, 10);
			noiseChanged |= OtUi::dragFloat("Amplitude", &noise.amplitude, 0.1f, 1.0f);
			noiseChanged |= OtUi::dragFloat("Persistence", &noise.persistence, 0.1f, 1.0f);
			noiseChanged |= OtUi::dragInt("Octaves##oct1", &noise.octaves, 1, 10);

			OtUi::header("Erosion");
			erosionChanged |= OtUi::dragFloat("Scale", &erosion.scale, 0.1f, 1.0f);
			erosionChanged |= OtUi::dragFloat("Strength##strength1", &erosion.strength, 0.1f, 1.0f);
			erosionChanged |= OtUi::dragFloat("Gully Weight", &erosion.gullyWeight, 0.0f, 1.0f);
			erosionChanged |= OtUi::dragFloat("Detail", &erosion.detail, 0.1f, 5.0f);
			erosionChanged |= OtUi::dragFloat("Normalization", &erosion.normalization, 0.0f, 1.0f);
			erosionChanged |= OtUi::dragInt("Octaves##oct2", &erosion.octaves, 1, 10);

			OtUi::header("Normals");
			normalsChanged |= OtUi::dragFloat("Strength##strength2", &normals.normalStrength, 1.0f, 100.0f);

			ImGui::PopItemWidth();
			ImGui::TableNextColumn();

			if (normalMap.isValid()) {
				ImGui::Image(normalMap.getTextureID(), ImVec2(512.0f, 512.0f));
			}

			ImGui::EndTable();

			textureDirty |= textureChanged;
			noiseDirty |= textureDirty | noiseChanged;
			erosionDirty |= noiseDirty | erosionChanged;
			normalsDirty |= erosionDirty | normalsChanged;
			changed = textureChanged | noiseChanged | erosionChanged | normalsChanged;
		}

		ImGui::EndPopup();
	}

	return changed;
}


//
//	OtTerrainHeights::serialize
//

nlohmann::json OtTerrainHeights::serialize([[maybe_unused]] std::string* basedir) {
	auto data = nlohmann::json::object();
	data["size"] = heightMapSize;

	data["noiseFrequency"] = noise.frequency;
	data["noiseLacunarity"] = noise.lacunarity;
	data["noiseAmplitude"] = noise.amplitude;
	data["noisePersistence"] = noise.persistence;
	data["noiseOctaves"] = noise.octaves;

	data["erosionRounding"] = erosion.rounding;
	data["erosionOnset"] = erosion.onset;
	data["erosionAssumedSlope"] = erosion.assumedSlope;
	data["erosionScale"] = erosion.scale;
	data["erosionStrength"] = erosion.strength;
	data["erosionGullyWeight"] = erosion.gullyWeight;
	data["erosionDetail"] = erosion.detail;
	data["erosionCellScale"] = erosion.cellScale;
	data["erosionGain"] = erosion.gain;
	data["erosionLacunarity"] = erosion.lacunarity;
	data["erosionNormalization"] = erosion.normalization;
	data["erosionOctaves"] = erosion.octaves;

	data["normalStrength"] = normals.normalStrength;

	return data;
}


//
//	OtTerrainHeights::deserialize
//

void OtTerrainHeights::deserialize(nlohmann::json& data, [[maybe_unused]] std::string* basedir) {
	heightMapSize = data.value("size", 256);
	heightMapDensity = static_cast<float>(heightMapSize) / 256.0f;

	noise.frequency = data.value("noiseFrequency", 10);
	noise.lacunarity = data.value("noiseLacunarity", 2);
	noise.amplitude = data.value("noiseAmplitude", 0.5f);
	noise.persistence = data.value("noisePersistence", 0.5f);
	noise.octaves = data.value("noiseOctaves", 5);

	erosion.rounding = data.value("erosionRounding", glm::vec4(0.1f, 0.0f, 0.1f, 2.0f));
	erosion.onset = data.value("erosionOnset", glm::vec4(1.25f, 1.25f, 2.8f, 1.5f));
	erosion.assumedSlope = data.value("erosionAssumedSlope", glm::vec2(0.7f, 1.0f));
	erosion.scale = data.value("erosionScale", 0.15f);
	erosion.strength = data.value("erosionStrength", 0.22f);
	erosion.gullyWeight = data.value("erosionGullyWeight", 0.5f);
	erosion.detail = data.value("erosierosionDetailon", 1.5f);
	erosion.cellScale = data.value("erosionCellScale", 0.7f);
	erosion.gain = data.value("erosionGain", 0.5f);
	erosion.lacunarity = data.value("erosionLacunarity", 2.0f);
	erosion.normalization = data.value("erosionNormalization", 0.5f);
	erosion.octaves = data.value("erosionOctaves", 5);

	normals.normalStrength = data.value("normalStrength", 10.0f);

	textureDirty = true;
	noiseDirty = true;
	erosionDirty = true;
	normalsDirty = true;
}


//
//	OtTerrainHeights::update
//

void OtTerrainHeights::update() {
	// update size of framebuffer
	noiseMap.update(heightMapSize, heightMapSize, OtTexture::Format::r32, OtTexture::Usage::rwDefault);
	erodedMap.update(heightMapSize, heightMapSize, OtTexture::Format::r32, OtTexture::Usage::rwDefault);
	normalMap.update(heightMapSize, heightMapSize, OtTexture::Format::rgba32, OtTexture::Usage::rwDefault);

	// create noise map (if required)
	if (noiseDirty) {
		noise.render(noiseMap);
		noiseDirty = false;
	}

	// create noise map (if required)
	if (erosionDirty) {
		erosion.render(noiseMap, erodedMap);
		erosionDirty = false;
	}

	// create normal map (if required)
	if (normalsDirty) {
		normals.render(erodedMap, normalMap);
		normalsDirty = false;
	}
}
