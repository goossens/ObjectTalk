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

	// update matrix
	OtPerspectiveCameraClass::updateViewMatrix();
}


//
//	OtFirstPersonCameraClass::renderGUI
//

bool OtFirstPersonCameraClass::renderGUI() {
	bool viewChanged = false;
	bool projectionChanged = false;

	viewChanged |= ImGui::InputFloat3("Position", glm::value_ptr(position));
	viewChanged |= ImGui::DragFloat("Distance", &distance, 1.0, 0.0, 0.0, "%.2f");
	viewChanged |= ImGui::DragFloat("Pitch", &pitch, 0.1, 0.0, 0.0, "%.2f");
	viewChanged |= ImGui::DragFloat("Yaw", &yaw, 0.1, 0.0, 0.0, "%.2f");

	if (viewChanged) {
		updateViewMatrix();
	}

	projectionChanged |= ImGui::SliderFloat("FoV (Deg)", &fov, 10, 160);
	projectionChanged |= ImGui::DragFloat("Near Clipping", &near, 1.0, 0.0, 0.0, ".0f");
	projectionChanged |= ImGui::DragFloat("Far Clipping", &far, 1.0, 0.0, 0.0, ".0f");

	if (projectionChanged) {
		OtPerspectiveCameraClass::updateProjectionMatrix();
	}

	return viewChanged || projectionChanged;
}


//
//	OtFirstPersonCameraClass::serialize
//

nlohmann::json OtFirstPersonCameraClass::serialize() {
	auto data = nlohmann::json::object();
	return data;
}


//
//	OtFirstPersonCameraClass::deserialize
//

void OtFirstPersonCameraClass::deserialize(nlohmann::json data) {
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
