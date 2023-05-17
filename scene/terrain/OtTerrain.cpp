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

#include "OtTerrain.h"
#include "OtTerrainConstants.h"


//
//	OtTerrain::OtTerrain
//

OtTerrain::OtTerrain() {
	// create the tiles

	// A B C D
	// D E F G
	// H I J K
	// L M N O

	auto half = OtTerrainBlockSize >> 1;
	auto quarter = OtTerrainBlockSize >> 2;

	// A B C D
	tiles.emplace_back(-half, -half, -quarter, -quarter);
	tiles.emplace_back(-quarter, -half, 0, -quarter);
	tiles.emplace_back(0, -half, quarter, -quarter);
	tiles.emplace_back(quarter, -half, half, -quarter);

	// D G
	tiles.emplace_back(-half, -quarter, -quarter, 0);
	tiles.emplace_back(quarter, -quarter, half, 0);

	// H K
	tiles.emplace_back(-half, 0, -quarter, quarter);
	tiles.emplace_back(quarter, 0, half, quarter);

	// L M N O
	tiles.emplace_back(-half, quarter, -quarter, half);
	tiles.emplace_back(-quarter, quarter, 0, half);
	tiles.emplace_back(0, quarter, quarter, half);
	tiles.emplace_back(quarter, quarter, half, half);

	// E F
	tiles.emplace_back(-quarter, -quarter, 0, 0);
	tiles.emplace_back(0, -quarter, quarter, 0);

	// I J
	tiles.emplace_back(-quarter, 0, 0, quarter);
	tiles.emplace_back(0, 0, quarter, quarter);

	// create meshes for all levels
	for (auto level = 0; level < OtTerrainMeshLevels; level++) {
		auto tilesPerLevel = level == 0 ? 16 : 12;

		for (auto t = 0;  t < tilesPerLevel; t++) {
			meshes.emplace_back(level, &tiles[t]);
		}
	}
}


//
//	OtTerrain::renderGUI
//

bool OtTerrain::renderGUI() {
	bool changed = false;
	changed |= ImGui::DragFloat("Horizontal Scale", &hScale, 0.5f, 5.0f);
	changed |= ImGui::DragFloat("Vertical Scale", &vScale, 1.0f, 100.0f);
	changed |= ImGui::Checkbox("Wireframe", &wireframe);
	return false;
}


//
//	OtTerrain::serialize
//

nlohmann::json OtTerrain::serialize(std::filesystem::path *basedir) {
	auto data = nlohmann::json::object();
	data["hScale"] = hScale;
	data["vScale"] = vScale;
	data["wireframe"] = wireframe;
	return data;
}


//
//	OtTerrain::deserialize
//

void OtTerrain::deserialize(nlohmann::json data, std::filesystem::path *basedir) {
	hScale = data.value("hScale", 1.0f);
	vScale = data.value("vScale", 1.0f);
	wireframe = data.value("wireframe", false);
}


//
//	OtTerrain::update
//

void OtTerrain::update(float hscale, float vscale, const glm::vec3& cameraPosition) {
	// determine camera offset
	float step = (1 << (OtTerrainMeshLevels - 1));
	xoffset = std::floor(cameraPosition.x / step) * step;
	zoffset = std::floor(cameraPosition.z / step) * step;

	// update all meshes
	for (auto& mesh : meshes) {
		mesh.update(hscale, vscale, xoffset, zoffset);
	}
}
