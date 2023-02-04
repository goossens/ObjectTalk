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

#include "OtGlm.h"

#include "OtFirstPersonCamera.h"


//
//	OtFirstPersonCameraClass::OtFirstPersonCameraClass
//

OtFirstPersonCameraClass::OtFirstPersonCameraClass() {
	updateViewMatrix();
	OtPerspectiveCameraClass::updateProjectionMatrix();
}


//
//	OtFirstPersonCameraClass::updateViewMatrix
//

void OtFirstPersonCameraClass::updateViewMatrix() {
	// calculate new forward vector
	auto pitchRad = glm::radians(pitch);
	auto yawRad = glm::radians(yaw);

	forward.x = std::cos(yawRad) * std::cos(pitchRad);
	forward.y = std::sin(pitchRad);
	forward.z = std::sin(yawRad) * std::cos(pitchRad);
	forward = glm::normalize(forward);

	// set new camera target based on forward vector
	target = position + forward;

	// calculate right vector
	right = glm::normalize(glm::cross(forward, up));

	// update matrix
	OtPerspectiveCameraClass::updateViewMatrix();
}


//
//	OtFirstPersonCameraClass::setDistance
//

void OtFirstPersonCameraClass::setDistance(float d) {
	distance = d;
	updateViewMatrix();
}


//
//	OtFirstPersonCameraClass::setPitch
//

void OtFirstPersonCameraClass::setPitch(float p) {
 	pitch = std::clamp(p, -89.9f, 89.9f);
	updateViewMatrix();
}


//
//	OtFirstPersonCameraClass::setYaw
//

void OtFirstPersonCameraClass::setYaw(float y) {
	yaw = y;

	if (yaw < -180.0f) {
		yaw += 360.0f;
	}

	if (yaw > 180.0f) {
		yaw -= 360.0f;
	}

	updateViewMatrix();
}


//
//	OtFirstPersonCameraClass::renderGUI
//

bool OtFirstPersonCameraClass::renderGUI() {
	bool viewChanged = false;
	bool projectionChanged = false;

	viewChanged |= ImGui::InputFloat3("Position", glm::value_ptr(position));
	viewChanged |= ImGui::DragFloat("Distance", &distance, 1.0f, 0.0f, 0.0f, "%.2f");
	viewChanged |= ImGui::DragFloat("Pitch (Deg)", &pitch, 0.1f, -89.9f, 89.9f, "%.2f");
	viewChanged |= ImGui::DragFloat("Yaw (Deg)", &yaw, 0.1f, 0.0f, 0.0f, "%.2f");

	if (viewChanged) {
		updateViewMatrix();
	}

	projectionChanged |= ImGui::DragFloat("FoV (Deg)", &fov, 1.0f, 10.0f, 160.0f, "%.0f");
	projectionChanged |= ImGui::DragFloat("Near Plane", &nearPlane, 1.0f, 0.0f, 0.0f, "%.1f");
	projectionChanged |= ImGui::DragFloat("Far Plane", &farPlane, 1.0f, 0.0f, 0.0f, "%.1f");

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
	data["type"] = name;

	data["position"] = position;
	data["distance"] = distance;
	data["pitch"] = pitch;
	data["yaw"] = yaw;
	data["up"] = up;

	data["fov"] = fov;
	data["near"] = nearPlane;
	data["far"] = farPlane;
	return data;
}


//
//	OtFirstPersonCameraClass::deserialize
//

void OtFirstPersonCameraClass::deserialize(nlohmann::json data) {
	position = data.value("position", glm::vec3(0.0f, 0.0f, 10.0f));
	distance = data.value("distance", 10.0f);
	pitch = data.value("pitch", 0.0f);
	yaw = data.value("yaw", 0.0f);
	up = data.value("up", glm::vec3(0.0f, 1.0f, 0.0f));

	fov = data.value("fov", 60.0f);
	nearPlane = data.value("near", 0.1f);
	farPlane = data.value("far", 1000.0f);

	updateViewMatrix();
	updateProjectionMatrix();
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
