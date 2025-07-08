//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtGlm.h"

#include "OtTerrainComponent.h"


//
//	OtTerrainComponent::OtTerrainComponent
//

OtTerrainComponent::OtTerrainComponent() {
	terrain = std::make_shared<OtTerrain>();
}


//
//	OtTerrainComponent::renderUI
//

bool OtTerrainComponent::renderUI() {
	return terrain->renderUI();
}


//
//	OtTerrainComponent::serialize
//

nlohmann::json OtTerrainComponent::serialize(std::string* basedir) {
	auto data = nlohmann::json::object();
	data.update(terrain->serialize(basedir));
	return data;
}


//
//	OtTerrainComponent::deserialize
//

void OtTerrainComponent::deserialize(nlohmann::json data, std::string* basedir) {
	terrain->deserialize(data, basedir);
}
