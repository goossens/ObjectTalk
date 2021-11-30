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
	cameraPosition = glm::vec3(x, y, z);
	return shared();
}


//
//	OtCameraClass::setTarget
//

OtObject OtCameraClass::setTarget(float x, float y, float z) {
	cameraTarget = glm::vec3(x, y, z);
	return shared();
}


//
//	OtCameraClass::setUp
//

OtObject OtCameraClass::setUp(float x, float y, float z) {
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
//	OtCameraClass::setScriptControlMode
//

OtObject OtCameraClass::setScriptControlMode() {
	mode = scriptControlCamera;
	return shared();
}


//
//	OtCameraClass::setCircleTargetMode
//

OtObject OtCameraClass::setCircleTargetMode() {
	mode = circleTargetCamera;
	return shared();
}


//
//	OtCameraClass::setFlyMode
//

OtObject OtCameraClass::setFirstPersonMode() {
	mode = firstPersonCamera;
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
//	OtCameraClass::setPitch
//

OtObject OtCameraClass::setPitch(float p) {
	pitch = p;
	pitch = std::clamp(pitch, pitchMin, pitchMax);
	return shared();
}


//
//	OtCameraClass::setYaw
//

OtObject OtCameraClass::setYaw(float y) {
	yaw = y;

	if (yaw < -std::numbers::pi) {
		yaw += std::numbers::pi * 2.0;
	}

	if (yaw > std::numbers::pi) {
		yaw -= std::numbers::pi * 2.0;
	}

	yaw = std::clamp(yaw, yawMin, yawMax);
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
//	OtCameraClass::setPitchLimits
//

OtObject OtCameraClass::setPitchLimits(float min, float max) {
	pitchMin = min;
	pitchMax = max;
	return shared();
}


//
//	OtCameraClass::setYawLimits
//

OtObject OtCameraClass::setYawLimits(float min, float max) {
	yawMin = min;
	yawMax = max;
	return shared();
}


//
//	OtCameraClass::setHeightLimits
//

OtObject OtCameraClass::setHeightLimits(float min, float max) {
	heightMin = min;
	heightMax = max;
	return shared();
}


//
//	OtCameraClass::onMouseDrag
//

bool OtCameraClass::onMouseDrag(int button, int mods, float xpos, float ypos) {
	if (mode != scriptControlCamera && button == 0 && mods == GLFW_MOD_CONTROL) {
		setPitch(pitch + ypos * 0.002);
		setYaw(yaw - xpos * 0.004);
		return true;
	}

	return false;
}


bool OtCameraClass::onScrollWheel(float dx, float dy) {
	if (mode == circleTargetCamera) {
		setDistance(distance - dy * 0.2);
		return true;

	} else if (mode == firstPersonCamera) {
		cameraPosition += dy * forward;
		return true;
	}

	return false;
}


//
//	OtCameraClass::onKey
//

bool OtCameraClass::onKey(int key, int mods) {
	switch (key) {
		case GLFW_KEY_UP:
			cameraPosition += forward;
			return true;

		case GLFW_KEY_DOWN:
			cameraPosition -= forward;
			return true;

		case GLFW_KEY_LEFT:
			cameraPosition -= right;
			return true;

		case GLFW_KEY_RIGHT:
			cameraPosition += right;
			return true;

		default:
			return false;
	}
}


//
//	OtCameraClass::update
//

void OtCameraClass::update(OtRenderingContext* context) {
	if (mode == circleTargetCamera) {
		// calculate new camera position
		cameraPosition = glm::vec3(
			cameraTarget.x + distance * std::cos(pitch) * std::sin(yaw),
			cameraTarget.y + distance * std::sin(pitch),
			cameraTarget.z + distance * std::cos(pitch) * std::cos(yaw));

	} else if (mode == firstPersonCamera) {
		// calculate new forward vector
		forward.x = std::cos(yaw) * std::cos(pitch);
        forward.y = std::sin(pitch);
        forward.z = std::sin(yaw) * std::cos(pitch);
        forward = glm::normalize(forward);

		// limit camera position
		cameraPosition.y = std::clamp(cameraPosition.y, heightMin, heightMax);

		// set new camera target based on forward vector
		cameraTarget = cameraPosition + forward;

		// calculate right vector
		right = glm::normalize(glm::cross(forward, cameraUp));
	}

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
	ImGui::SliderFloat("FoV (Deg)", &fov, 10.0f, 120.0f);
	ImGui::SliderFloat("Near Clipping", &nearClip, 0.1f, 10.0f);
	ImGui::SliderFloat("Far Clipping", &farClip, 10.0f, 2000.0f);

	if (mode == scriptControlCamera) {
		ImGui::SliderFloat3("Position", glm::value_ptr(cameraPosition), -50.0f, 50.0f);
		ImGui::SliderFloat3("Target", glm::value_ptr(cameraTarget), -50.0f, 50.0f);
		ImGui::SliderFloat3("Up", glm::value_ptr(cameraUp), -2.0f, 2.0f);

	} else if (mode == circleTargetCamera) {
		ImGui::SliderFloat3("Target", glm::value_ptr(cameraTarget), -50.0f, 50.0f);
		ImGui::SliderFloat("Distance", &distance, distanceMin, distanceMax);
		ImGui::SliderFloat("Pitch", &pitch, pitchMin, pitchMax);
		ImGui::SliderFloat("Yaw", &yaw, yawMin, yawMax);

	} else if (mode == firstPersonCamera) {
		ImGui::SliderFloat3("Position", glm::value_ptr(cameraPosition), -50.0f, 50.0f);
		ImGui::SliderFloat("Pitch", &pitch, pitchMin, pitchMax);
		ImGui::SliderFloat("Yaw", &yaw, yawMin, yawMax);

	} else {
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

		type->set("setScriptControlMode", OtFunctionClass::create(&OtCameraClass::setScriptControlMode));
		type->set("setCircleTargetMode", OtFunctionClass::create(&OtCameraClass::setCircleTargetMode));
		type->set("setFirstPersonMode", OtFunctionClass::create(&OtCameraClass::setFirstPersonMode));

		type->set("setDistance", OtFunctionClass::create(&OtCameraClass::setDistance));
		type->set("setPitch", OtFunctionClass::create(&OtCameraClass::setPitch));
		type->set("setYaw", OtFunctionClass::create(&OtCameraClass::setYaw));
		type->set("setDistanceLimits", OtFunctionClass::create(&OtCameraClass::setDistanceLimits));
		type->set("setPitchLimits", OtFunctionClass::create(&OtCameraClass::setPitchLimits));
		type->set("setYawLimits", OtFunctionClass::create(&OtCameraClass::setYawLimits));
		type->set("setHeightLimits", OtFunctionClass::create(&OtCameraClass::setHeightLimits));
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
