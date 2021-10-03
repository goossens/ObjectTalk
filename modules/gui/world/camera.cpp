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

#include "ot/function.h"

#include "application.h"
#include "camera.h"


//
//	OtCameraClass::setPosition
//

OtObject OtCameraClass::setPosition(float x, float y, float z) {
	cameraPosition = glm::vec3(x, y, z);
	return shared();
}


//
//	OtCameraClass::setTarget
//

OtObject OtCameraClass::setTarget(float x, float y, float z) {
	if (mouseControl) {
		OtExcept("Camera is under mouse control and can't be adjusted");
	}

	cameraTarget = glm::vec3(x, y, z);
	return shared();
}


//
//	OtCameraClass::setUp
//

OtObject OtCameraClass::setUp(float x, float y, float z) {
	if (mouseControl) {
		OtExcept("Camera is under mouse control and can't be adjusted");
	}

	cameraUp = glm::vec3(x, y, z);
	return shared();
}


//
//	OtCameraClass::setFOV
//

OtObject OtCameraClass::setFOV(float f) {
	fov = f;
	return shared();
}


//
//	OtCameraClass::setClipping
//

OtObject OtCameraClass::setClipping(float near, float far) {
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

OtObject OtCameraClass::setDistance(float d) {
	distance = std::clamp(d, distanceMin, distanceMax);
	return shared();
}


//
//	OtCameraClass::setAngle
//

OtObject OtCameraClass::setAngle(float a) {
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

OtObject OtCameraClass::setPitch(float p) {
	pitch = p;
	pitch = std::clamp(pitch, pitchMin, pitchMax);
	return shared();
}


//
//	OtCameraClass::setDistanceLimits
//

OtObject OtCameraClass::setDistanceLimits(float min, float max) {
	distanceMin = min;
	distanceMax = max;
	return shared();
}


//
//	OtCameraClass::setAngleLimits
//

OtObject OtCameraClass::setAngleLimits(float min, float max) {
	angleMin = min;
	angleMax = max;
	return shared();
}


//
//	OtCameraClass::setPitchLimits
//

OtObject OtCameraClass::setPitchLimits(float min, float max) {
	pitchMin = min;
	pitchMax = max;
	return shared();
}


//
//	OtCameraClass::onMouseDrag
//

bool OtCameraClass::onMouseDrag(int button, int mods, float xpos, float ypos) {
	if (mouseControl && button == 0 && mods == GLFW_MOD_CONTROL) {
		setAngle(angle - xpos * 0.004);
		setPitch(pitch + ypos * 0.002);
		return true;
	}

	return false;
}


bool OtCameraClass::onScrollWheel(float dx, float dy) {
	if (mouseControl) {
		setDistance(distance - dy * 0.2);
		return true;
	}

	return false;
}


//
//	OtCameraClass::submit
//

void OtCameraClass::submit(int view, float viewAspect) {
	// update camera position in mouse control mode
	if (mouseControl) {
		cameraPosition = glm::vec3(
			target.x + distance * std::cos(pitch) * std::sin(angle),
			target.y + distance * std::sin(pitch),
			target.z + distance * std::cos(pitch) * std::cos(angle));
	}

	// determine view and projection transformations
	viewMatrix = glm::lookAt(cameraPosition, cameraTarget, cameraUp);

	projMatrix = glm::perspective(
		(float) glm::radians(fov),
		(float) viewAspect,
		(float) nearClip,
		(float) farClip);

	// determine frustum planes (in world space)
	glm::mat4 mat = projMatrix * viewMatrix;
	planes[0] = glm::row(mat, 3) + glm::row(mat, 0); // left
	planes[1] = glm::row(mat, 3) - glm::row(mat, 0); // right
	planes[2] = glm::row(mat, 3) + glm::row(mat, 1); // bottom
	planes[3] = glm::row(mat, 3) - glm::row(mat, 1); // top
	planes[4] = glm::row(mat, 3) + glm::row(mat, 2); // far
	planes[5] = glm::row(mat, 3) - glm::row(mat, 2); // near

	// setup BGFX
	bgfx::setViewTransform(view, glm::value_ptr(viewMatrix), glm::value_ptr(projMatrix));
}


//
//	OtCameraClass::isVisiblePoint
//

bool OtCameraClass::isVisiblePoint(const glm::vec3& point) {
	// check against all frustum planes
	for (auto c = 0; c < 6; c++) {
		if (glm::dot(planes[c].normal, point) + planes[c].d > 0) {
			return false;
		 }
	}

	return true;
}


//
//	OtCameraClass::isVisibleAABB
//

bool OtCameraClass::isVisibleAABB(const glm::vec3& min, const glm::vec3& max) {
	// check against all frustum planes
	for (auto c = 0; c < 6; c++) {
		glm::vec3 point (
			planes[c].normal.x > 0 ? min.x : max.x,
			planes[c].normal.y > 0 ? min.y : max.y,
			planes[c].normal.z > 0 ? min.z : max.z);

		if (glm::dot(planes[c].normal, point) + planes[c].d > 0) {
			return false;
		}
	}

	return true;
}


//
//	OtCameraClass::isVisibleSphere
//

bool OtCameraClass::isVisibleSphere(const glm::vec3& center, float radius) {
	// check against all frustum planes
	for (auto c = 0; c < 6; c++) {
		if (glm::dot(planes[c].normal, center) + planes[c].d + radius > 0) {
			return false;
		}
	}

	return true;
}


//
//	OtCameraClass::renderGUI
//

void OtCameraClass::renderGUI() {
	if (mouseControl) {
		ImGui::SliderFloat3("Target", glm::value_ptr(cameraTarget), -50.0f, 50.0f);
		ImGui::SliderFloat("Distance", &distance, distanceMin, distanceMax);
		ImGui::SliderFloat("Angle", &angle, angleMin, angleMax);
		ImGui::SliderFloat("Pitch", &pitch, pitchMin, pitchMax);

	} else {
		ImGui::SliderFloat3("Position", glm::value_ptr(cameraPosition), -50.0f, 50.0f);
		ImGui::SliderFloat3("Target", glm::value_ptr(cameraTarget), -50.0f, 50.0f);
		ImGui::SliderFloat3("Up", glm::value_ptr(cameraUp), -2.0f, 2.0f);
	    ImGui::SliderFloat("FoV (Deg)", &fov, 10.0f, 120.0f);
	}
}


//
//	OtCameraClass::getMeta
//

OtType OtCameraClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtCameraClass>("Camera", OtGuiClass::getMeta());
		type->set("setPosition", OtFunctionClass::create(&OtCameraClass::setPosition));
		type->set("setTarget", OtFunctionClass::create(&OtCameraClass::setTarget));
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
