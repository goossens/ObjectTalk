//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cmath>

#include "imgui.h"

#include "OtFirstPersonCamera.h"


//
//	OtFirstPersonCameraClass::updateViewMatrix
//

void OtFirstPersonCameraClass::updateViewMatrix() {
	// calculate new forward vector
	forward.x = std::cos(yaw) * std::cos(pitch);
	forward.y = std::sin(pitch);
	forward.z = std::sin(yaw) * std::cos(pitch);
	forward = glm::normalize(forward);

	// set new camera target based on forward vector
	target = position + forward;

	// calculate right vector
	right = glm::normalize(glm::cross(forward, up));

	// update matrices
	OtPerspectiveCameraClass::updateViewMatrix();
}


//
//	OtFirstPersonCameraClass::renderGUI
//

void OtFirstPersonCameraClass::renderGUI() {
	bool changed = false;

	if (ImGui::SliderFloat("FoV (Deg)", &fov, 10, 160)) { changed = true; }
	if (ImGui::DragFloat("Near Clipping", &near, 1.0, 0.0, 0.0, ".0f")) { changed = true; }
	if (ImGui::DragFloat("Far Clipping", &far, 1.0, 0.0, 0.0, ".0f")) { changed = true; }

	if (ImGui::DragFloat("Distance", &distance, 1.0, 0.0, 0.0, "%.2f")) { changed = true; }
	if (ImGui::DragFloat("Pitch", &pitch, 0.1, 0.0, 0.0, "%.2f")) { changed = true; }
	if (ImGui::DragFloat("Yaw", &yaw, 0.1, 0.0, 0.0, "%.2f")) { changed = true; }

	if (changed) {
		updateViewMatrix();
	}
}


//
//	OtFirstPersonCameraClass::getMeta
//

OtType OtFirstPersonCameraClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtFirstPersonCameraClass>("FirstPersonCamera", OtPerspectiveCameraClass::getMeta());
	}

	return type;
}


//
//	OtFirstPersonCameraClass::create
//

OtFirstPersonCamera OtFirstPersonCameraClass::create() {
	OtFirstPersonCamera camera = std::make_shared<OtFirstPersonCameraClass>();
	camera->setType(getMeta());
	return camera;
}
