//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>
#include <cmath>

#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtCameraComponent.h"


//
//	OtCameraComponent::renderUI
//

bool OtCameraComponent::renderUI() {
	bool changed = false;
	changed |= ImGui::Checkbox("Main Camera", &mainCamera);
	changed |= ImGui::DragFloat("FoV (Deg)", &fov, 1.0f, 10.0f, 160.0f, "%.0f");
	changed |= ImGui::DragFloat("Near Plane", &nearPlane, 1.0f, 0.0f, 0.0f, "%.1f");
	changed |= ImGui::DragFloat("Far Plane", &farPlane, 1.0f, 0.0f, 0.0f, "%.1f");
	return changed;
}


//
//	OtCameraComponent::serialize
//

nlohmann::json OtCameraComponent::serialize(std::filesystem::path* basedir) {
	auto data = nlohmann::json::object();
	data["component"] = name;
	data["mainCamera"] = mainCamera;
	data["fov"] = fov;
	data["near"] = nearPlane;
	data["far"] = farPlane;
	return data;
}


//
//	OtCameraComponent::deserialize
//

void OtCameraComponent::deserialize(nlohmann::json data, std::filesystem::path* basedir) {
	mainCamera = data.value("mainCamera", false);
	fov = data.value("fov", 60.0f);
	nearPlane = data.value("near", 0.1f);
	farPlane = data.value("far", 100.0f);
}
