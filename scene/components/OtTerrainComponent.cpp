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
