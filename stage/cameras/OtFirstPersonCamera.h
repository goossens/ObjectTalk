//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "glm/glm.hpp"
#include "nlohmann/json_fwd.hpp"

#include "OtController.h"
#include "OtPerspectiveCamera.h"


//
//	FirstPerson camera class
//

class OtFirstPersonCameraClass;
typedef std::shared_ptr<OtFirstPersonCameraClass> OtFirstPersonCamera;

class OtFirstPersonCameraClass : public OtPerspectiveCameraClass {
public:
	// setters
	void setDistance(float d);
	void setPitch(float p);
	void setYaw(float y);

	// getters
	float getDistance() { return distance; }
	float getPitch() { return pitch; }
	float getYaw() { return yaw; }
	glm::vec3& getForward() { return forward; }
	glm::vec3& getRight() { return right; }

	// GUI to change camera properties
	bool renderGUI() override;

	// (de)serialize component
	nlohmann::json serialize() override;
	void deserialize(nlohmann::json data) override;

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtFirstPersonCamera create();

	// get type name of camera
	const char* getTypeName() override { return name; }

	// camera name
	static constexpr char const* name = "First Person Perspective";

private:
	// update the view matrix when camera settings are changed
	void updateViewMatrix();

	// properties
	float distance = 10.0f;
	float pitch = 0.0f;
	float yaw = 0.0f;

	// derived values
	glm::vec3 forward = { 0.0f, 0.0f, 1.0f };
	glm::vec3 right= { 1.0f, 0.0f, 0.0f };
};
