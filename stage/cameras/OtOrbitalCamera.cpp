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

#include "OtOrbitalCamera.h"


//
//	OtOrbitalCameraClass::updateViewMatrix
//

void OtOrbitalCameraClass::updateViewMatrix() {
	// calculate new camera position
	position = glm::vec3(
		target.x + distance * std::cos(pitch) * std::sin(yaw),
		target.y + distance * std::sin(pitch),
		target.z + distance * std::cos(pitch) * std::cos(yaw));

	OtPerspectiveCameraClass::updateViewMatrix();
}


//
//	OtOrbitalCameraClass::renderGUI
//

bool OtOrbitalCameraClass::renderGUI() {
	bool viewChanged = false;
	bool projectionChanged = false;

	viewChanged |= ImGui::InputFloat3("Target", glm::value_ptr(target));
	viewChanged |= ImGui::DragFloat("Distance", &distance, 1.0, 0.0, 0.0, "%.2f");
	viewChanged |= ImGui::DragFloat("Pitch", &pitch, 0.1, 0.0, 0.0, "%.2f");
	viewChanged |= ImGui::DragFloat("Yaw", &yaw, 0.1, 0.0, 0.0, "%.2f");

	if (viewChanged) {
		updateViewMatrix();
	}

	projectionChanged |= ImGui::SliderFloat("FoV (Deg)", &fov, 10, 160);
	projectionChanged |= ImGui::DragFloat("Near Plane", &near, 1.0, 0.0, 0.0, ".0f");
	projectionChanged |= ImGui::DragFloat("Far Plane", &far, 1.0, 0.0, 0.0, ".0f");

	if (projectionChanged) {
		OtPerspectiveCameraClass::updateProjectionMatrix();
	}

	return viewChanged || projectionChanged;
}


//
//	OtOrbitalCameraClass::serialize
//

nlohmann::json OtOrbitalCameraClass::serialize() {
	auto data = nlohmann::json::object();
	return data;
}


//
//	OtOrbitalCameraClass::deserialize
//

void OtOrbitalCameraClass::deserialize(nlohmann::json data) {
}


//
//	OtOrbitalCameraClass::getMeta
//

OtType OtOrbitalCameraClass::getMeta()
{
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtOrbitalCameraClass>("OrbitalCamera", OtPerspectiveCameraClass::getMeta());
	}

	return type;
}

//
//	OtOrbitalCameraClass::create
//

OtOrbitalCamera OtOrbitalCameraClass::create() {
	OtOrbitalCamera camera = std::make_shared<OtOrbitalCameraClass>();
	camera->setType(getMeta());
	return camera;
}
