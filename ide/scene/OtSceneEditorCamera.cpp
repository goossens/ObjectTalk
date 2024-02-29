//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>

#include <glm/gtc/matrix_transform.hpp>
#include "imgui.h"

#include "OtGpu.h"

#include "OtSceneEditorCamera.h"


//
//	Constants
//

static constexpr float maxMovementPerSecond = 3.0f;
static constexpr float maxRotationPerSecond = 10.0f;
static constexpr float maxZoomPerSecond = 10.0f;


//
//	OtSceneEditorCamera::update
//

void OtSceneEditorCamera::update() {
	// calculate the new forward vector
	forward = glm::normalize(glm::vec3(
		std::cos(glm::radians(pitch)) * std::sin(glm::radians(yaw)),
		std::sin(glm::radians(pitch)),
		std::cos(glm::radians(pitch)) * -std::cos(glm::radians(yaw))));

	// also re-calculate the right and up vector
	right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));
	up = glm::normalize(glm::cross(right, forward));
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
			yaw -= drag.x * maxRotationPerSecond * delta;
			ImGui::ResetMouseDragDelta();
		}

		// determine maximum move and zoom changes
		auto movementStep = ImGui::IsKeyDown(ImGuiMod_Shift) ? maxMovementPerSecond * delta * 5.0f : maxMovementPerSecond * delta;

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
//	OtSceneEditorCamera::setPreset
//

void OtSceneEditorCamera::setPreset(int p) {
	// remember new preset and set camera properties
	preset = p;

	if (preset == tinyScenePreset) {
		position = glm::vec3(0.0f, 2.0f, 5.0f);
		pitch = -20.0f;
		yaw = 0.0f;
		fov = 60.0f;
		nearPlane = 0.1f;
		farPlane = 50.0f;

	} else if (preset == smallScenePreset) {
		position = glm::vec3(0.0f, 5.0f, 20.0f);
		pitch = -20.0f;
		yaw = 0.0f;
		fov = 60.0f;
		nearPlane = 0.5f;
		farPlane = 200.0f;

	} else if (preset == mediumScenePreset) {
		position = glm::vec3(0.0f, 10.0f, 50.0f);
		pitch = -20.0f;
		yaw = 0.0f;
		fov = 60.0f;
		nearPlane = 1.0f;
		farPlane = 500.0f;

	} else if (preset == largeScenePreset) {
		position = glm::vec3(0.0f, 20.0f, 200.0f);
		pitch = -20.0f;
		yaw = 0.0f;
		fov = 60.0f;
		nearPlane = 1.0f;
		farPlane = 2000.0f;

	} else if (preset == hugeScenePreset) {
		position = glm::vec3(0.0f, 50.0f, 500.0f);
		pitch = -20.0f;
		yaw = 0.0f;
		fov = 60.0f;
		nearPlane = 1.0f;
		farPlane = 5000.0f;
	}
}


//
//	OtSceneEditorCamera::getViewMatrix
//

glm::mat4 OtSceneEditorCamera::getViewMatrix() {
	return glm::lookAt(position, position + forward, up);
}
