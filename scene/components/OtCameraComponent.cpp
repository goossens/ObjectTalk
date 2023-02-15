//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>
#include <cmath>

#include "glm/ext.hpp"
#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtGpu.h"

#include "OtCameraComponent.h"

//
//	OtCameraComponent::getProjectionMatrix
//

glm::mat4 OtCameraComponent::getProjectionMatrix(float aspectRatio) {
	return OtGpuHasHomogeneousDepth()
		? glm::perspectiveRH_NO(glm::radians(fov), aspectRatio, nearPlane, farPlane)
		: glm::perspectiveRH_ZO(glm::radians(fov), aspectRatio, nearPlane, farPlane);
}


//
//	OtCameraComponent::renderGUI
//

bool OtCameraComponent::renderGUI() {
	bool changed = false;
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
	data["fov"] = fov;
	data["near"] = nearPlane;
	data["far"] = farPlane;
	return data;
}


//
//	OtCameraComponent::deserialize
//

void OtCameraComponent::deserialize(nlohmann::json data, std::filesystem::path* basedir) {
	fov = data.value("fov", 60.0f);
	nearPlane = data.value("near", 0.1f);
	farPlane = data.value("far", 100.0f);
}
