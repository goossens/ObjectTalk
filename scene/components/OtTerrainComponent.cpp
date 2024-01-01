//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
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

#include "OtTerrainComponent.h"


//
//	OtTerrainComponent::OtTerrainComponent
//

OtTerrainComponent::OtTerrainComponent() {
	terrain = OtObjectPointer<OtTerrainClass>::create();
}


//
//	OtTerrainComponent::renderGUI
//

bool OtTerrainComponent::renderGUI() {
	return terrain->renderGUI();
}


//
//	OtTerrainComponent::serialize
//

nlohmann::json OtTerrainComponent::serialize(std::filesystem::path* basedir) {
	auto data = nlohmann::json::object();
	data["component"] = name;
	data.update(terrain->serialize(basedir));
	return data;
}


//
//	OtTerrainComponent::deserialize
//

void OtTerrainComponent::deserialize(nlohmann::json data, std::filesystem::path* basedir) {
	terrain->deserialize(data, basedir);
}
