//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "nlohmann/json_fwd.hpp"

#include "OtController.h"
#include "OtPerspectiveCamera.h"


//
//	Orbital camera class
//

class OtOrbitalCameraClass;
typedef std::shared_ptr<OtOrbitalCameraClass> OtOrbitalCamera;

class OtOrbitalCameraClass : public OtPerspectiveCameraClass {
public:
	// setters
	void setDistance(float d);
	void setPitch(float p);
	void setYaw(float y);

	// getters
	float getDistance() { return distance; }
	float getPitch() { return pitch; }
	float getYaw() { return yaw; }

	// GUI to change camera properties
	bool renderGUI() override;

	// mouse/keyboard events
	void handleMouseKeyboard() override;

	// (de)serialize component
	nlohmann::json serialize() override;
	void deserialize(nlohmann::json data) override;

	// get type name of camera
	const char* getTypeName() override { return name; }

	// camera name
	static constexpr char const* name = "Orbital Perspective";

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtOrbitalCamera create();

private:
	// update the view matrix when camera settings are changed
	void updateViewMatrix();

	// properties
	float distance = 10.0f;
	float pitch = 0.0f;
	float yaw = 0.0f;
};
