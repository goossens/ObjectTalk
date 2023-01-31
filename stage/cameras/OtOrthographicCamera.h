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
//	Orthographic camera class
//

class OtOrthographicCameraClass;
typedef std::shared_ptr<OtOrthographicCameraClass> OtOrthographicCamera;

class OtOrthographicCameraClass : public OtCamera2Class {
public:
	// setters
	void setFrustum(float left, float right, float top, float bottom, float near, float far);

	// getters
	float getLeft() { return left; }
	float getRight() { return right; }
	float getTop() { return top; }
	float getBottom() { return bottom; }
	float getNear() { return near; }
	float getFar() { return far; }

	// GUI to change camera properties
	bool renderGUI() override;

	// (de)serialize component
	nlohmann::json serialize() override;
	void deserialize(nlohmann::json data) override;

	// get type name of camera
	const char* getTypeName() override { return name; }

	// camera name
	static constexpr char const* name = "Orthograpic";

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtOrthographicCamera create();

protected:
	// update the matrices when camera settings are changed
	void updateProjectionMatrix();

	// properties
	float left = -8.0f;
	float right = 8.0f;
	float top = 4.5f;
	float bottom = -4.5f;
	float near = 0.1f;
	float far = 1000.0f;
};
