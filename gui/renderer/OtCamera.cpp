//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glm/ext.hpp"
#include "imgui.h"

#include "OtFunction.h"

#include "OtCamera.h"
#include "OtGpu.h"


//
//	OtCameraClass::setPerspective
//

OtObject OtCameraClass::setPerspective(float fv, float n, float f) {
	style = perspectiveStyle;
	fov = fv;
	near = n;
	far = f;
	changed = true;
	return shared();
}


//
//	OtCameraClass::setOrthographic
//

OtObject OtCameraClass::setOrthographic(float w, float n, float f) {
	style = orthographicStyle;
	width = w;
	near = n;
	far = f;
	changed = true;
	return shared();
}

OtObject OtCameraClass::setOrthographicCustom(float xmn, float xmx, float ymn, float ymx, float zmn, float zmx) {
	style = orthographicCustomStyle;
	xmin = xmn;
	xmax = xmx;
	ymin = ymn;
	ymax = ymx;
	zmin = zmn;
	zmax = zmx;
	changed = true;
	return shared();
}


//
//	OtCameraClass::setFovLimits
//

OtObject OtCameraClass::setFovLimits(float fmn, float fmx) {
	fovMin = fmn;
	fovMax = fmx;
	return shared();
}


//
//	OtCameraClass::setWidthLimits
//mn

OtObject OtCameraClass::setWidthLimits(float wmn, float wmx) {
	widthMin = wmn;
	widthMax = wmx;
	return shared();
}


//
//	OtCameraClass::setNearFarLimits
//

OtObject OtCameraClass::setNearFarLimits(float nmn, float nmx, float fmn, float fmx) {
	nearMin = nmn;
	nearMax = nmx;
	farMin = fmn;
	farMax = fmx;
	return shared();
}


//
//	OtCameraClass::setScriptControlMode
//

OtObject OtCameraClass::setScriptControlMode() {
	mode = scriptControlMode;
	return shared();
}


//
//	OtCameraClass::setCircleTargetMode
//

OtObject OtCameraClass::setCircleTargetMode() {
	mode = circleTargetMode;
	return shared();
}


//
//	OtCameraClass::setFlyMode
//

OtObject OtCameraClass::setFirstPersonMode() {
	mode = firstPersonMode;
	return shared();
}


//
//	OtCameraClass::setPosition
//

OtObject OtCameraClass::setPosition(float x, float y, float z) {
	cameraPosition = glm::vec3(x, y, z);
	changed = true;
	return shared();
}


//
//	OtCameraClass::setTarget
//

OtObject OtCameraClass::setTarget(float x, float y, float z) {
	cameraTarget = glm::vec3(x, y, z);
	changed = true;
	return shared();
}


//
//	OtCameraClass::setUp
//

OtObject OtCameraClass::setUp(float x, float y, float z) {
	cameraUp = glm::vec3(x, y, z);
	changed = true;
	return shared();
}


//
//	OtCameraClass::setPositionVector
//

void OtCameraClass::setPositionVector(const glm::vec3& position) {
	cameraPosition = position;
	changed = true;
}


//
//	OtCameraClass::setTargetVector
//

void OtCameraClass::setTargetVector(const glm::vec3& target) {
	cameraTarget = target;
	changed = true;
}


//
//	OtCameraClass::setUpVector
//

void OtCameraClass::setUpVector(const glm::vec3& up) {
	cameraUp = up;
	changed = true;
}


//
//	OtCameraClass::setAspectRatio
//

void OtCameraClass::setAspectRatio(float ar) {
	if (ar != aspectRatio) {
		aspectRatio = ar;
	changed = true;
	}
}


//
//	OtCameraClass::setDistance
//

OtObject OtCameraClass::setDistance(float d) {
	distance = std::clamp(d, distanceMin, distanceMax);
	changed = true;
	return shared();
}


//
//	OtCameraClass::setPitch
//

OtObject OtCameraClass::setPitch(float p) {
	pitch = p;
	pitch = std::clamp(pitch, pitchMin, pitchMax);
	changed = true;
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
	changed = true;
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
	if (mode != scriptControlMode && button == 0 && mods == GLFW_MOD_CONTROL) {
		setPitch(pitch + ypos * 0.002);
		setYaw(yaw - xpos * 0.004);
		return true;
	}

	return false;
}


//
//	OtCameraClass::onScrollWheel
//

bool OtCameraClass::onScrollWheel(float dx, float dy) {
	if (mode == circleTargetMode) {
		setDistance(distance - dy * 0.2);
		return true;

	} else if (mode == firstPersonMode) {
		setPositionVector(cameraPosition + dy * forward);
		return true;
	}

	return false;
}


//
//	OtCameraClass::onKey
//

bool OtCameraClass::onKey(int key, int mods) {
	if (mode == firstPersonMode) {
		switch (key) {
			case GLFW_KEY_UP:
				setPositionVector(cameraPosition + forward);
				return true;

			case GLFW_KEY_DOWN:
				setPositionVector(cameraPosition - forward);
				return true;

			case GLFW_KEY_LEFT:
				setPositionVector(cameraPosition - right);
				return true;

			case GLFW_KEY_RIGHT:
				setPositionVector(cameraPosition + right);
				return true;

			case GLFW_KEY_HOME:
				setPositionVector(glm::vec3(0.0, cameraPosition.y, 0.0));
				return true;
		}
	}

	return false;
}


//
//	OtCameraClass::update
//

void OtCameraClass::update() {
	if (mode == circleTargetMode) {
		// calculate new camera position
		cameraPosition = glm::vec3(
			cameraTarget.x + distance * std::cos(pitch) * std::sin(yaw),
			cameraTarget.y + distance * std::sin(pitch),
			cameraTarget.z + distance * std::cos(pitch) * std::cos(yaw));

	} else if (mode == firstPersonMode) {
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

	// determine projection matrix for camera
	if (style == perspectiveStyle) {
		projMatrix = OtGpuHasHomogeneousDepth()
			? glm::perspectiveRH_NO(glm::radians(fov), aspectRatio, near, far)
			: glm::perspectiveRH_ZO(glm::radians(fov), aspectRatio, near, far);

	} else if (style == orthographicStyle) {
		float w = width / 2.0;
		float h = w / aspectRatio;

		projMatrix = OtGpuHasHomogeneousDepth()
			? glm::orthoRH_NO(-w, w, -h, h, near, far)
			: glm::orthoRH_ZO(-w, w, -h, h, near, far);

	} else {
		projMatrix = OtGpuHasHomogeneousDepth()
			? glm::orthoRH_NO(xmin, xmax, ymin, ymax, zmin, zmax)
			: glm::orthoRH_ZO(xmin, xmax, ymin, ymax, zmin, zmax);
	}

	// calculate combined view and projection matrix
	viewProjMatrix = projMatrix * viewMatrix;

	// update frustum (in world space)
	frustum = OtFrustum(viewProjMatrix);
}


//
//	OtCameraClass::isVisiblePoint
//

bool OtCameraClass::isVisiblePoint(const glm::vec3& point) {
	return frustum.isVisiblePoint(point);
}


//
//	OtCameraClass::isVisibleAABB
//

bool OtCameraClass::isVisibleAABB(OtAABB aabb) {
	return frustum.isVisibleAABB(aabb);
}


//
//	OtCameraClass::isVisibleSphere
//

bool OtCameraClass::isVisibleSphere(const glm::vec3& center, float radius) {
	return frustum.isVisibleSphere(center, radius);
}


//
//	OtCameraClass::renderGUI
//

void OtCameraClass::renderGUI() {
	if (style == perspectiveStyle) {
		if (ImGui::SliderFloat("FoV (Deg)", &fov, fovMin, fovMax)) { changed = true; }

	} else if (style == orthographicStyle) {
		if (ImGui::SliderFloat("Width", &width, widthMin, widthMax)) { changed = true; }
	}

	if (ImGui::SliderFloat("Near Plane", &near, nearMin, nearMax)) { changed = true; }
	if (ImGui::SliderFloat("Far Plane", &far, farMin, farMax)) { changed = true; }

	if (mode == scriptControlMode) {
		if (ImGui::InputFloat3("Position", glm::value_ptr(cameraPosition))) { changed = true; }
		if (ImGui::InputFloat3("Target", glm::value_ptr(cameraTarget))) { changed = true; }
		if (ImGui::SliderFloat3("Up", glm::value_ptr(cameraUp), -1.0f, 1.0f)) { changed = true; }

	} else if (mode == circleTargetMode) {
		if (ImGui::InputFloat3("Target", glm::value_ptr(cameraTarget))) { changed = true; }
		if (ImGui::SliderFloat("Distance", &distance, distanceMin, distanceMax)) { changed = true; }
		if (ImGui::SliderFloat("Pitch", &pitch, pitchMin, pitchMax)) { changed = true; }
		if (ImGui::SliderFloat("Yaw", &yaw, yawMin, yawMax)) { changed = true; }

	} else if (mode == firstPersonMode) {
		if (ImGui::InputFloat3("Position", glm::value_ptr(cameraPosition))) { changed = true; }
		if (ImGui::SliderFloat("Pitch", &pitch, pitchMin, pitchMax)) { changed = true; }
		if (ImGui::SliderFloat("Yaw", &yaw, yawMin, yawMax)) { changed = true; }

	} else {
	}
}


//
//	OtCameraClass::getMeta
//

OtType OtCameraClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtCameraClass>("Camera", OtGuiClass::getMeta());

		type->set("setOrthographic", OtFunctionClass::create(&OtCameraClass::setOrthographic));
		type->set("setPerspective", OtFunctionClass::create(&OtCameraClass::setPerspective));
		type->set("setNearFarLimits", OtFunctionClass::create(&OtCameraClass::setNearFarLimits));

		type->set("setScriptControlMode", OtFunctionClass::create(&OtCameraClass::setScriptControlMode));
		type->set("setCircleTargetMode", OtFunctionClass::create(&OtCameraClass::setCircleTargetMode));
		type->set("setFirstPersonMode", OtFunctionClass::create(&OtCameraClass::setFirstPersonMode));

		type->set("setPosition", OtFunctionClass::create(&OtCameraClass::setPosition));
		type->set("setTarget", OtFunctionClass::create(&OtCameraClass::setTarget));
		type->set("setUp", OtFunctionClass::create(&OtCameraClass::setUp));

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

OtCamera OtCameraClass::create(OtCamera camera) {
	OtCamera clone = create();

	clone->cameraPosition = camera->cameraPosition;
	clone->cameraTarget = camera->cameraTarget;
	clone->cameraUp = camera->cameraUp;
	clone->distance = camera->distance;
	clone->pitch = camera->pitch;
	clone->yaw = camera->yaw;
	clone->distanceMin = camera->distanceMin;
	clone->distanceMax = camera->distanceMax;
	clone->pitchMin = camera->pitchMin;
	clone->pitchMax = camera->pitchMax;
	clone->yawMin = camera->yawMin;
	clone->yawMax = camera->yawMax;
	clone->heightMin = camera->heightMin;
	clone->heightMax = camera->heightMax;
	clone->fov = camera->fov;
	clone->width = camera->width;
	clone->near = camera->near;
	clone->far = camera->far;
	clone->aspectRatio = camera->aspectRatio;

	return clone;
}
