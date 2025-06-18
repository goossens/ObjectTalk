//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "glm/glm.hpp"
#include "nlohmann/json_fwd.hpp"


//
//	OtSceneEditorCamera
//

class OtSceneEditorCamera {
public:
	// update the camera (called each frame)
	void update();

	// render a user interface for the camera
	bool renderUI();

	// (de)serialize data
	nlohmann::json serialize(std::string* basedir);
	void deserialize(nlohmann::json data, std::string* basedir);

	// process possible camera inputs
	void handleKeyboardAndMouse();

	// property setters
	void setPosition(const glm::vec3& p) { position = p; };
	void setPitch(float p) { pitch = p; };
	void setYaw(float y) { yaw = y; };
	void setFov(float f) { fov = f; };
	void setNearPlane(float n) { nearPlane = n; };
	void setFarPlane(float f) { farPlane = f; };

	// property getters
	glm::vec3 getPosition() { return position; }
	float getPitch() { return pitch; }
	float getYaw() { return yaw; }
	float getFov() { return fov; }
	float getNearPlane() { return nearPlane; }
	float getFarPlane() { return farPlane; }

	// get derived information
	glm::mat4 getViewMatrix();

private:
	// camera properties
	glm::vec3 position{0.0f, 2.0f, 5.0f};
	float pitch = -20.0f;
	float yaw = 0.0f;

	float fov = 60.0f;
	float nearPlane = 0.1f;
	float farPlane = 100.0f;
	float speed = 3.0f;

	// constants
	static constexpr float maxRotationPerSecond = 10.0f;
	static constexpr float maxZoomPerSecond = 10.0f;

	// work properties
	glm::vec3 forward;
	glm::vec3 right;
	glm::vec3 up;
};
