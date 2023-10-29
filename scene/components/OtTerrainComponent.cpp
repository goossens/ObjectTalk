//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cmath>

#include "glm/ext.hpp"
#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtGlm.h"

#include "OtTerrainComponent.h"


//
//	OtTerrainComponent::OtTerrainComponent
//

OtTerrainComponent::OtTerrainComponent() {
}


//
//	OtTerrainComponent::renderGUI
//

bool OtTerrainComponent::renderGUI() {
	bool changed = false;
	changed |= ImGui::DragFloat("Horizontal Scale", &hScale, 0.5f, 5.0f);
	changed |= ImGui::DragFloat("Vertical Scale", &vScale, 1.0f, 100.0f);

#if OT_DEBUG
	changed |= ImGui::Checkbox("Wireframe", &wireframe);
#endif

	return changed;
}


//
//	OtTerrainComponent::serialize
//

nlohmann::json OtTerrainComponent::serialize(std::filesystem::path* basedir) {
	auto data = nlohmann::json::object();
	data["component"] = name;
	data["hScale"] = hScale;
	data["vScale"] = vScale;
	data["vOffset"] = vOffset;
	data["wireframe"] = wireframe;
	return data;
}


//
//	OtTerrainComponent::deserialize
//

void OtTerrainComponent::deserialize(nlohmann::json data, std::filesystem::path* basedir) {
	hScale = data.value("hScale", 1.0f);
	vScale = data.value("vScale", 1.0f);
	vOffset = data.value("vOffset", 0.5f);
	wireframe = data.value("wireframe", false);
}


//
//	OtTerrainComponent::getTiles
//

void OtTerrainComponent::getTiles(glm::vec3 center, OtFrustum& frustum) {
	// determine number of tiles visible from center
	auto visibleFromCenter = (int) std::ceil(maxViewingDist / OtTerrainTileSize);

	// determine center tile
	auto terrainCenterTileX = (int) std::floor(center.x / maxViewingDist);
	auto terrainCenterTileY = (int) std::floor(center.z / maxViewingDist);

	// get vertical limits
	auto minHeight = vOffset * vScale;
	auto maxHeight = (1.0f + vOffset) * vScale;

	// process all possible visible tiles
	tiles.clear();

	for (auto yOffset = -visibleFromCenter; yOffset <= visibleFromCenter; yOffset++) {
		for (auto xOffset = -visibleFromCenter; xOffset <= visibleFromCenter; xOffset++) {
			auto x = terrainCenterTileX + xOffset;
			auto y = terrainCenterTileY + yOffset;

			// ensure tile is visible in camera frustum
			auto cx = x * OtTerrainTileSize;
			auto cy = y * OtTerrainTileSize;

			OtAABB aabb;
			aabb.addPoint(glm::vec3(cx, minHeight, cy));
			aabb.addPoint(glm::vec3(cx + OtTerrainTileSize, maxHeight, cy + OtTerrainTileSize));

			if (frustum.isVisibleAABB(aabb)) {
				tiles.emplace_back();
			}
		}
	}
}
