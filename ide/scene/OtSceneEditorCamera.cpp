//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>

#include "glm/gtc/matrix_transform.hpp"
#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtGlm.h"
#include "OtGpu.h"
#include "OtUi.h"

#include "OtSceneEditorCamera.h"


//
//	Constants
//

static constexpr float maxRotationPerSecond = 10.0f;
static constexpr float maxZoomPerSecond = 10.0f;


//
//	OtSceneEditorCamera::update
//

void OtSceneEditorCamera::update() {
	// calculate the new forward vector
	forward = glm::normalize(glm::vec3(
		std::cos(glm::radians(pitch)) * -std::sin(glm::radians(yaw)),
		std::sin(glm::radians(pitch)),
		std::cos(glm::radians(pitch)) * -std::cos(glm::radians(yaw))));

	// also re-calculate the right and up vector
	right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));
	up = glm::normalize(glm::cross(right, forward));
}


//
//	OtSceneEditorCamera::renderUI
//

bool OtSceneEditorCamera::renderUI() {
	bool changed = false;
	changed |= OtUiEditVec3("Position", &position);
	changed |= OtUiDragFloat("Pitch (Deg)", &pitch, -90.0f, 90.0f);
	changed |= OtUiDragFloat("Yaw (Deg)", &yaw, -180.0f, 180.0f);

	changed |= OtUiDragFloat("FoV (Deg)", &fov, 10.0f, 160.0f);
	changed |= OtUiDragFloat("Near Plane", &nearPlane, 0.01f);
	changed |= OtUiDragFloat("Far Plane", &farPlane, nearPlane);

	changed |= OtUiDragFloat("Movement Speed", &speed, 1.0f, 10.0f);

	if (ImGui::BeginMenu("Presets")) {
		if (ImGui::MenuItem("Tiny Scene")) {
			position = glm::vec3(0.0f, 2.0f, 5.0f);
			pitch = -20.0f;
			yaw = 0.0f;
			fov = 60.0f;
			nearPlane = 0.1f;
			farPlane = 50.0f;
			changed = true;

		} else if (ImGui::MenuItem("Small Scene")) {
			position = glm::vec3(0.0f, 5.0f, 20.0f);
			pitch = -20.0f;
			yaw = 0.0f;
			fov = 60.0f;
			nearPlane = 0.5f;
			farPlane = 200.0f;
			changed = true;

		} else if (ImGui::MenuItem("Medium Scene")) {
			position = glm::vec3(0.0f, 10.0f, 50.0f);
			pitch = -20.0f;
			yaw = 0.0f;
			fov = 60.0f;
			nearPlane = 1.0f;
			farPlane = 500.0f;
			changed = true;

		} else if (ImGui::MenuItem("Large Scene")) {
			position = glm::vec3(0.0f, 20.0f, 200.0f);
			pitch = -20.0f;
			yaw = 0.0f;
			fov = 60.0f;
			nearPlane = 1.0f;
			farPlane = 2000.0f;
			changed = true;

		} else if (ImGui::MenuItem("Huge Scene")) {
			position = glm::vec3(0.0f, 50.0f, 500.0f);
			pitch = -20.0f;
			yaw = 0.0f;
			fov = 60.0f;
			nearPlane = 1.0f;
			farPlane = 5000.0f;
			changed = true;
		}

		ImGui::EndMenu();
	}

	return changed;
}


//
//	OtSceneEditorCamera::serialize
//

nlohmann::json OtSceneEditorCamera::serialize(std::string* basedir) {
	auto data = nlohmann::json::object();
	data["position"] = position;
	data["pitch"] = pitch;
	data["yaw"] = yaw;
	data["fov"] = fov;
	data["near"] = nearPlane;
	data["far"] = farPlane;
	data["speed"] = speed;
	return data;
}


//
//	OtSceneEditorCamera::deserialize
//

void OtSceneEditorCamera::deserialize(nlohmann::json data, std::string* basedir) {
	position = data.value("position", glm::vec3(0.0f, 2.0f, 5.0f));
	pitch = data.value("pitch", -20.0f);
	yaw = data.value("yaw", 0.0f);
	fov = data.value("fov", 60.0f);
	nearPlane = data.value("near", 0.1f);
	farPlane = data.value("far", 100.0f);
	speed = data.value("speed", 3.0f);
}


//
//	OtSceneEditorCamera::handleKeyboardAndMouse
//

void OtSceneEditorCamera::handleKeyboardAndMouse() {
	if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
		// determine timestep
		float delta = ImGui::GetIO().DeltaTime;

		// handle mouse interactions
		ImVec2 drag = ImGui::GetMouseDragDelta();

		if (drag.x != 0.0 || drag.y != 0.0) {
			pitch += drag.y * maxRotationPerSecond * delta / 2.0f;
			yaw += drag.x * maxRotationPerSecond * delta;
			ImGui::ResetMouseDragDelta();
		}

		// determine maximum move and zoom changes
		auto movementStep = ImGui::IsKeyDown(ImGuiMod_Shift) ? speed * delta * 5.0f : speed * delta;

		if (ImGui::IsKeyDown(ImGuiKey_W) || ImGui::IsKeyDown(ImGuiKey_Keypad8) || ImGui::IsKeyDown(ImGuiKey_UpArrow)) {
			position += movementStep * forward;

		} else if (ImGui::IsKeyDown(ImGuiKey_S) || ImGui::IsKeyDown(ImGuiKey_Keypad2) || ImGui::IsKeyDown(ImGuiKey_DownArrow)) {
			position -= movementStep * forward;

		} else if (ImGui::IsKeyDown(ImGuiKey_A) || ImGui::IsKeyDown(ImGuiKey_Keypad4) || ImGui::IsKeyDown(ImGuiKey_LeftArrow)) {
			position -= movementStep * right;

		} else if (ImGui::IsKeyDown(ImGuiKey_D) || ImGui::IsKeyDown(ImGuiKey_Keypad6) || ImGui::IsKeyDown(ImGuiKey_RightArrow)) {
			position += movementStep * right;

		} else if (ImGui::IsKeyDown(ImGuiKey_E) || ImGui::IsKeyDown(ImGuiKey_Keypad9) || ImGui::IsKeyDown(ImGuiKey_PageUp)) {
			position += movementStep * up;

		} else if (ImGui::IsKeyDown(ImGuiKey_Q) || ImGui::IsKeyDown(ImGuiKey_Keypad7) || ImGui::IsKeyDown(ImGuiKey_PageDown)) {
			position -= movementStep * up;

		} else if (ImGui::IsKeyDown(ImGuiKey_Z) || ImGui::IsKeyDown(ImGuiKey_Minus) || ImGui::IsKeyDown(ImGuiKey_Keypad1)) {
			fov -= maxZoomPerSecond * delta;

		} else if (ImGui::IsKeyDown(ImGuiKey_C) || ImGui::IsKeyDown(ImGuiKey_Equal) ||  ImGui::IsKeyDown(ImGuiKey_Keypad3)) {
			fov += maxZoomPerSecond * delta;
		}
	}

	// ensure all properties are in range
 	pitch = std::clamp(pitch, -89.9f, 89.9f);

	if (yaw < -360.0f) {
		yaw += 360.0f;
	}

	if (yaw > 360.0f) {
		yaw -= 360.0f;
	}

 	fov = std::clamp(fov, 20.0f, 160.0f);
}


//
//	OtSceneEditorCamera::getViewMatrix
//

glm::mat4 OtSceneEditorCamera::getViewMatrix() {
	return glm::lookAt(position, position + forward, up);
}
