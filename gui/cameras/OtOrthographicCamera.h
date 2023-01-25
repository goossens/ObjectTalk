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
	void setWidth(float width);
	void setAspectRatio(float aspectRatio);
	void setNearFar(float n, float f) { near = n; far = f; updateProjectionMatrix(); }
	void setFrustum(float left, float right, float top, float bottom, float near, float far);

	// getters
	float getWidth() { return width; }
	float getAspectRatio() { return aspectRatio; }
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

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtOrthographicCamera create();

	// camera name
	static constexpr char const* name = "Orthograpic";

protected:
	// update the matrices when camera settings are changed
	void updateProjectionMatrix();

	// properties
	float width = 16.0;
	float aspectRatio = 16.0 / 9.0;
	float left = -8.0;
	float right = 8.0;
	float top = 4.5;
	float bottom = -4.5;
	float near = 0.1;
	float far = 1000.0;
};
