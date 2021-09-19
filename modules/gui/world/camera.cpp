//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>

#include "bgfx/bgfx.h"
#include "imgui.h"

#include "ot/function.h"

#include "application.h"
#include "camera.h"


//
//	OtCameraClass::setPosition
//

OtObject OtCameraClass::setPosition(double x, double y, double z) {
	cameraPos = glm::vec3(x, y, z);
	return shared();
}


//
//	OtCameraClass::setDirection
//

OtObject OtCameraClass::setDirection(double x, double y, double z) {
	if (mouseControl) {
		OtExcept("Camera is under mouse control and can't be adjusted");
	}

	cameraDir = glm::vec3(x, y, z);
	return shared();
}


//
//	OtCameraClass::setUp
//

OtObject OtCameraClass::setUp(double x, double y, double z) {
	if (mouseControl) {
		OtExcept("Camera is under mouse control and can't be adjusted");
	}

	cameraUp = glm::vec3(x, y, z);
	return shared();
}


//
//	OtCameraClass::setFOV
//

OtObject OtCameraClass::setFOV(double f) {
	fov = f;
	return shared();
}


//
//	OtCameraClass::setClipping
//

OtObject OtCameraClass::setClipping(double near, double far) {
	nearClip = near;
	farClip = far;
	return shared();
}


//
//	OtCameraClass::setMouseControl
//

OtObject OtCameraClass::setMouseControl(bool control) {
	mouseControl = control;
	return shared();
}


//
//	OtCameraClass::setDistance
//

OtObject OtCameraClass::setDistance(double d) {
	distance = d;
	distance = std::clamp(distance, distanceMin, distanceMax);
	return shared();
}


//
//	OtCameraClass::setAngle
//

OtObject OtCameraClass::setAngle(double a) {
	angle = a;

	if (angle < -std::numbers::pi) {
		angle += std::numbers::pi * 2.0;
	}

	if (angle > std::numbers::pi) {
		angle -= std::numbers::pi * 2.0;
	}

	angle = std::clamp(angle, angleMin, angleMax);
	return shared();
}


//
//	OtCameraClass::setPitch
//

OtObject OtCameraClass::setPitch(double p) {
	pitch = p;
	pitch = std::clamp(pitch, pitchMin, pitchMax);
	return shared();
}


//
//	OtCameraClass::setDistanceLimits
//

OtObject OtCameraClass::setDistanceLimits(double min, double max) {
	distanceMin = min;
	distanceMax = max;
	return shared();
}


//
//	OtCameraClass::setAngleLimits
//

OtObject OtCameraClass::setAngleLimits(double min, double max) {
	angleMin = min;
	angleMax = max;
	return shared();
}


//
//	OtCameraClass::setPitchLimits
//

OtObject OtCameraClass::setPitchLimits(double min, double max) {
	pitchMin = min;
	pitchMax = max;
	return shared();
}


//
//	OtCameraClass::onMouseDrag
//

bool OtCameraClass::onMouseDrag(int button, int mods, double xpos, double ypos) {
	if (mouseControl && button == 0 && mods == GLFW_MOD_CONTROL) {
		setAngle(angle - xpos * 0.004);
		setPitch(pitch + ypos * 0.002);
		return true;
	}

	return false;
}


bool OtCameraClass::onScrollWheel(double dx, double dy) {
	if (mouseControl) {
		setDistance(distance - dy * 0.2);
		return true;
	}

	return false;
}


//
//	OtCameraClass::renderGUI
//

void OtCameraClass::renderGUI() {
	if (mouseControl) {
		OtExcept("Camera is under mouse control and can't be adjusted");
	}

	ImGui::SliderFloat3("Position", glm::value_ptr(cameraPos), -50.0f, 50.0f);
	ImGui::SliderFloat3("Target", glm::value_ptr(cameraDir), -50.0f, 50.0f);
	ImGui::SliderFloat3("Up", glm::value_ptr(cameraUp), -2.0f, 2.0f);
    ImGui::SliderFloat("FoV (Deg)", &fov, 10.0f, 120.0f);
}


//
//	OtCameraClass::submit
//

void OtCameraClass::submit(int view, float viewAspect) {
	// update camera position in mouse control mode
	if (mouseControl) {
		cameraPos = glm::vec3(
			target.x + distance * std::cos(pitch) * std::sin(angle),
			target.y + distance * std::sin(pitch),
			target.z + distance * std::cos(pitch) * std::cos(angle));

		cameraDir = target;
		cameraUp = glm::vec3(0.0, 1.0, 0.0);
	}

	// determine view and projection transformations
	viewMatrix = glm::lookAt(cameraPos, cameraDir, cameraUp);


	projMatrix = glm::perspective(
		(float) glm::radians(fov),
		(float) viewAspect,
		(float) nearClip,
		(float) farClip);

	// setup BGFX
	bgfx::setViewTransform(view, glm::value_ptr(viewMatrix), glm::value_ptr(projMatrix));
}


//
//	OtCameraClass::getMeta
//

OtType OtCameraClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtCameraClass>("Camera", OtGuiClass::getMeta());
		type->set("setPosition", OtFunctionClass::create(&OtCameraClass::setPosition));
		type->set("setDirection", OtFunctionClass::create(&OtCameraClass::setDirection));
		type->set("setUp", OtFunctionClass::create(&OtCameraClass::setUp));
		type->set("setFOV", OtFunctionClass::create(&OtCameraClass::setFOV));
		type->set("setClipping", OtFunctionClass::create(&OtCameraClass::setClipping));
		type->set("setMouseControl", OtFunctionClass::create(&OtCameraClass::setMouseControl));
		type->set("setDistance", OtFunctionClass::create(&OtCameraClass::setDistance));
		type->set("setAngle", OtFunctionClass::create(&OtCameraClass::setAngle));
		type->set("setPitch", OtFunctionClass::create(&OtCameraClass::setPitch));
		type->set("setDistanceLimits", OtFunctionClass::create(&OtCameraClass::setDistanceLimits));
		type->set("setAngleLimits", OtFunctionClass::create(&OtCameraClass::setAngleLimits));
		type->set("setPitchLimits", OtFunctionClass::create(&OtCameraClass::setPitchLimits));
	}

	return type;
}


//
//	OtCameraClass::create
//

OtCamera OtCameraClass::create() {
	OtCamera camera = std::make_shared<OtCameraClass>();
	camera->setType(getMeta());
	return camera;
}
