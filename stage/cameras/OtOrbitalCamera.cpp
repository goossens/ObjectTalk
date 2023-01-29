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

#include "OtOrbitalCamera.h"


//
//	OtOrbitalCameraClass::updateViewMatrix
//

void OtOrbitalCameraClass::updateViewMatrix() {
	auto pitchRad = glm::radians(pitch);
	auto yawRad = glm::radians(yaw);

	// calculate new camera position
	position = glm::vec3(
		target.x + distance * std::cos(pitchRad) * std::sin(yawRad),
		target.y + distance * std::sin(pitchRad),
		target.z + distance * std::cos(pitchRad) * std::cos(yawRad));

	OtPerspectiveCameraClass::updateViewMatrix();
}


//
//	OtOrbitalCameraClass::setDistance
//

void OtOrbitalCameraClass::setDistance(float d) {
	distance = d;
	updateViewMatrix();
}


//
//	OtOrbitalCameraClass::setPitch
//

void OtOrbitalCameraClass::setPitch(float p) {
 	pitch = std::clamp(p, -89.9f, 89.9f);
	updateViewMatrix();
}


//
//	OtOrbitalCameraClass::setYaw
//

void OtOrbitalCameraClass::setYaw(float y) {
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
//	OtOrbitalCameraClass::renderGUI
//

bool OtOrbitalCameraClass::renderGUI() {
	bool viewChanged = false;
	bool projectionChanged = false;

	viewChanged |= ImGui::InputFloat3("Target", glm::value_ptr(target));
	viewChanged |= ImGui::DragFloat("Distance", &distance, 1.0f, 0.0f, 0.0f, "%.1f");
	viewChanged |= ImGui::DragFloat("Pitch (Deg)", &pitch, 1.0f, -89.9f, 89.9f, "%.1f");
	viewChanged |= ImGui::DragFloat("Yaw (Deg)", &yaw, 1.0, -180.0f, 180.0f, "%.1f");

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
//	OtOrbitalCameraClass::handleMouseKeyboard
//

void OtOrbitalCameraClass::handleMouseKeyboard() {
	if (ImGui::IsMouseDown(0)) {
		ImVec2 drag = ImGui::GetMouseDragDelta();
		setPitch(pitch + drag.y * 0.15f);
		setYaw(yaw - drag.x * 0.3f);
		ImGui::ResetMouseDragDelta();

	} else if (ImGui::IsKeyPressed(ImGuiKey_MouseWheelY)) {
		setDistance(distance - ImGui::GetIO().MouseWheel * 0.2f);

	} else if (ImGui::IsKeyPressed(ImGuiKey_UpArrow)) {

	} else if (ImGui::IsKeyPressed(ImGuiKey_DownArrow)) {

	} else if (ImGui::IsKeyPressed(ImGuiKey_LeftArrow)) {

	} else if (ImGui::IsKeyPressed(ImGuiKey_RightArrow)) {

	} else if (ImGui::IsKeyPressed(ImGuiKey_Minus)) {
		setFov(getFov() + 2.0f);

	} else if (ImGui::IsKeyPressed(ImGuiKey_Equal)) {
		setFov(getFov() - 2.0f);

	} else if (ImGui::IsKeyPressed(ImGuiKey_Home)) {
		setPitch(0.0f);
		setYaw(0.0f);
		setFov(60.0f);
	}
}


//
//	OtOrbitalCameraClass::serialize
//

nlohmann::json OtOrbitalCameraClass::serialize() {
	auto data = nlohmann::json::object();
	data["type"] = name;

	data["target"] = target;
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
//	OtOrbitalCameraClass::deserialize
//

void OtOrbitalCameraClass::deserialize(nlohmann::json data) {
	target = data["target"];
	distance = data["distance"];
	pitch = data["pitch"];
	yaw = data["yaw"];
	up = data["up"];

	fov = data["fov"];
	nearPlane = data["near"];
	farPlane = data["far"];

	updateViewMatrix();
	updateProjectionMatrix();
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
