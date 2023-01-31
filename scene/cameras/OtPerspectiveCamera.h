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

#include "OtCamera2.h"
#include "OtController.h"


//
//	Perspective camera class
//

class OtPerspectiveCameraClass;
typedef std::shared_ptr<OtPerspectiveCameraClass> OtPerspectiveCamera;

class OtPerspectiveCameraClass : public OtCamera2Class {
public:
	// setters
	void setFov(float f) { fov = f; updateProjectionMatrix(); }
	void setAspectRatio(float a) { aspectRatio = a; updateProjectionMatrix(); }
	void setNearFar(float n, float f) { nearPlane = n; farPlane = f; updateProjectionMatrix(); }

	// getters
	float getFov() { return fov; }
	float getAspectRatio() { return aspectRatio; }
	float getNear() { return nearPlane; }
	float getFar() { return farPlane; }

	// GUI to change camera properties
	bool renderGUI() override;

	// (de)serialize component
	nlohmann::json serialize() override;
	void deserialize(nlohmann::json data) override;

	// get type name of camera
	const char* getTypeName() override { return name; }

	// camera name
	static constexpr char const* name = "Perspective";

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtPerspectiveCamera create();

protected:
	// update the matrices when camera settings are changed
	void updateProjectionMatrix();

	// properties
	float fov = 60.0f;
	float nearPlane = 0.1f;
	float farPlane = 1000.0f;

	float aspectRatio;
};
