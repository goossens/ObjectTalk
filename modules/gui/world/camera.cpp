//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>

#include "ot/function.h"

#include "application.h"
#include "camera.h"


//
//	OtCameraClass::setPosition
//

OtObject OtCameraClass::setPosition(float x, float y, float z) {
	if (mouseControl) {
		OtExcept("Camera is under mouse control and can't be adjusted");
	}

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
	updatePosition();
	return shared();
}


//
//	OtCameraClass::setDistance
//

OtObject OtCameraClass::setDistance(float d) {
	distance = std::clamp(d, distanceMin, distanceMax);
	updatePosition();
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
	updatePosition();
	return shared();
}


//
//	OtCameraClass::setPitch
//

OtObject OtCameraClass::setPitch(float p) {
	pitch = p;
	pitch = std::clamp(pitch, pitchMin, pitchMax);
	updatePosition();
	return shared();
}


//
//	OtCameraClass::updatePosition
//

void OtCameraClass::updatePosition() {
	cameraPosition = glm::vec3(
		target.x + distance * std::cos(pitch) * std::sin(angle),
		target.y + distance * std::sin(pitch),
		target.z + distance * std::cos(pitch) * std::cos(angle));
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
//	OtCameraClass::update
//

void OtCameraClass::update(OtRenderingContext* context) {
	// determine transformations
	viewMatrix = glm::lookAt(cameraPosition, cameraTarget, cameraUp);
	projMatrix = glm::perspective(glm::radians(fov), context->viewAspect, nearClip, farClip);
	viewProjMatrix = projMatrix * viewMatrix;
	invViewProjMatrix = glm::inverse(viewProjMatrix);

	// determine frustum (in world space)
	frustum = OtFrustum(viewProjMatrix);
}


//
//	OtCameraClass::submit
//

void OtCameraClass::submit(OtRenderingContext* context) {
	bgfx::setViewTransform(context->view, glm::value_ptr(viewMatrix), glm::value_ptr(projMatrix));
}


//
//	OtCameraClass::isVisiblePoint
//

bool OtCameraClass::isVisiblePoint(const glm::vec3& point) {
	// check against all frustum planes
/*
	for (auto c = 0; c < 6; c++) {
		if (glm::dot(planes[c].normal, point) + planes[c].d > 0) {
			return false;
		 }
	}
*/
	return true;
}


//
//	OtCameraClass::isVisibleAABB
//

bool OtCameraClass::isVisibleAABB(const glm::vec3& min, const glm::vec3& max) {
	return frustum.isVisibleAABB(min, max);
}


//
//	OtCameraClass::isVisibleSphere
//

bool OtCameraClass::isVisibleSphere(const glm::vec3& center, float radius) {
	// check against all frustum planes
/*
	for (auto c = 0; c < 6; c++) {
		if (glm::dot(planes[c].normal, center) + planes[c].d + radius > 0) {
			return false;
		}
	}
*/

	return true;
}


//
//	OtCameraClass::getDirectionFromNDC
//

glm::vec3 OtCameraClass::getDirectionFromNDC(float x, float y) {
	glm::vec4 ndcPos = glm::vec4(x, y, 1.0, 1.0);
	glm::vec4 worldPos = invViewProjMatrix * ndcPos;
	glm::vec3 dir = glm::normalize(glm::vec3(worldPos));
	return dir;
}


//
//	OtCameraClass::renderGUI
//

void OtCameraClass::renderGUI() {
	if (mouseControl) {
		ImGui::SliderFloat3("Target", glm::value_ptr(cameraTarget), -50.0f, 50.0f);

		if (ImGui::SliderFloat("Distance", &distance, distanceMin, distanceMax)) {
			updatePosition();
		}

		if (ImGui::SliderFloat("Angle", &angle, angleMin, angleMax)) {
			updatePosition();
		}

		if (ImGui::SliderFloat("Pitch", &pitch, pitchMin, pitchMax)) {
			updatePosition();
		}

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
