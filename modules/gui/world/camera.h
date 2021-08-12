//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "glm/ext.hpp"
#include "glm/glm.hpp"

#include "gui.h"


//
//	OtCameraClass
//

class OtCameraClass;
typedef std::shared_ptr<OtCameraClass> OtCamera;

class OtCameraClass : public OtGuiClass {
	friend class OtViewClass;

public:
	// initialize camera
	void init(double x, double y, double z, double dx, double dy, double dz, double ux, double uy, double uz);

	// change camera geometry
	void setPosition(double x, double y, double z) { cameraPos = glm::vec3(x, y, z); }
	void setDirection(double x, double y, double z) { cameraDir = glm::vec3(x, y, z); }
	void setUp(double x, double y, double z) { cameraUp = glm::vec3(x, y, z); }

	void setFOV(double _f) { fov = _f; }
	void setClipping(double near, double far) { nearClip = near; farClip = far; }

	// GUI to change camera parameters
	void renderGUI();

	// submit data to BGFX
	void submit(int view, float viewAspect);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtCamera create();

private:
	// camera geometry
	glm::vec3 cameraPos = { 0.0, 0.0, 10.0 };
	glm::vec3 cameraDir = { 0.0, 0.0, 0.0 };
	glm::vec3 cameraUp = { 0.0, 1.0, 0.0 };

	// field-of-view and clipping
	float fov = 60.0;
	float nearClip = 0.1;
	float farClip = 100;

	// transformation matrices;
	glm::mat4 viewMatrix;
	glm::mat4 projMatrix;
};
