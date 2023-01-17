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

void OtOrbitalCameraClass::renderGUI() {
	bool changed = false;

	if (ImGui::SliderFloat("FoV (Deg)", &fov, 10, 160)) { changed = true; }
	if (ImGui::DragFloat("Near Clipping", &near, 1.0, 0.0, 0.0, ".0f")) { changed = true; }
	if (ImGui::DragFloat("Far Clipping", &far, 1.0, 0.0, 0.0, ".0f")) { changed = true; }

	if (ImGui::InputFloat3("Target", glm::value_ptr(target))) { changed = true; }
	if (ImGui::DragFloat("Distance", &distance, 1.0, 0.0, 0.0, "%.2f")) { changed = true; }
	if (ImGui::DragFloat("Pitch", &pitch, 0.1, 0.0, 0.0, "%.2f")) { changed = true; }
	if (ImGui::DragFloat("Yaw", &yaw, 0.1, 0.0, 0.0, "%.2f")) { changed = true; }

	if (changed) {
		updateViewMatrix();
	}
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
