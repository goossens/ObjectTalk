//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtController.h"
#include "OtPerspectiveCamera.h"

#include "glm/glm.hpp"


//
//	FirstPerson camera class
//

class OtFirstPersonCameraClass;
typedef std::shared_ptr<OtFirstPersonCameraClass> OtFirstPersonCamera;

class OtFirstPersonCameraClass : public OtPerspectiveCameraClass {
public:
	// setters
	void setDistance(float d) { distance = d; updateViewMatrix(); }
	void setPitch(float p) { pitch = p; updateViewMatrix(); }
	void setYaw(float y) { yaw = y; updateViewMatrix(); }

	// getters
	float getDistance() { return distance; }
	float getPitch() { return pitch; }
	float getYaw() { return yaw; }
	glm::vec3& getForward() { return forward; }
	glm::vec3& getRight() { return right; }

	// GUI to change camera properties
	void renderGUI() override;

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtFirstPersonCamera create();

private:
	// update the view matrix when camera settings are changed
	void updateViewMatrix();

	// properties
	float distance = 10.0;
	float pitch = 0.0;
	float yaw = 0.0;

	// derived values
	glm::vec3 forward = { 0.0, 0.0, 1.0 };
	glm::vec3 right= { 1.0, 0.0, 0.0 };
};
