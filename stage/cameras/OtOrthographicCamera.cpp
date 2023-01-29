//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "glm/ext.hpp"
#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtGlm.h"

#include "OtGpu.h"
#include "OtOrthographicCamera.h"


//
//	OtOrthographicCameraClass::setFrustum
//

void OtOrthographicCameraClass::setFrustum(float l, float r, float t, float b, float n, float f) {
	left = l;
	right = r;
	top = t;
	bottom = b;
	near = n;
	far = f;

	updateProjectionMatrix();
}


//
//	OtOrthographicCameraClass::updateProjectionMatrix
//

void OtOrthographicCameraClass::updateProjectionMatrix() {
	projMatrix = OtGpuHasHomogeneousDepth()
		? glm::orthoRH_NO(left, right, bottom, top, near, far)
		: glm::orthoRH_ZO(left, right, bottom, top, near, far);

	OtCamera2Class::updateDerived();
}


//
//	OtOrthographicCameraClass::renderGUI
//

bool OtOrthographicCameraClass::renderGUI() {
	bool changed = OtCamera2Class::renderGUI();

	auto centerX = (left + right) / 2.0f;
	auto centerY = (bottom + top) / 2.0f;
	auto width = right - left;
	auto aspectRatio = width / (top - bottom);

	changed |= ImGui::DragFloat("Width", &width, 1.0f, 0.0f, 0.0f, "%.0f");

	if (changed) {
		left = centerX - width / 2.0f;
		right = centerX + width / 2.0f;
		auto height = width / aspectRatio;
		bottom = centerY - height / 2.0f;
		top = centerY + height / 2.0f;
	}

	changed |= ImGui::DragFloat("Near Plane", &near, 1.0f, 0.0f, 0.0f, "%.1f");
	changed |= ImGui::DragFloat("Far Plane", &far, 1.0f, 0.0f, 0.0f, "%.1f");

	if (changed) {
		updateProjectionMatrix();
	}

	return changed;
}


//
//	OtOrthographicCameraClass::serialize
//

nlohmann::json OtOrthographicCameraClass::serialize() {
	auto data = nlohmann::json::object();
	data["type"] = name;

	data["position"] = target;
	data["target"] = target;
	data["up"] = up;

	data["left"] = left;
	data["right"] = right;
	data["top"] = top;
	data["bottom"] = bottom;
	data["near"] = near;
	data["far"] = far;

	return data;
}


//
//	OtOrthographicCameraClass::deserialize
//

void OtOrthographicCameraClass::deserialize(nlohmann::json data) {
	position = data["position"];
	target = data["target"];
	up = data["up"];

	left = data["left"];
	right = data["right"];
	top = data["top"];
	bottom = data["bottom"];
	near = data["near"];
	far = data["far"];

	updateViewMatrix();
	updateProjectionMatrix();
}


//
//	OtOrthographicCameraClass::getMeta
//

OtType OtOrthographicCameraClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtOrthographicCameraClass>("OrthographicCamera", OtCamera2Class::getMeta());
	}

	return type;
}


//
//	OtOrthographicCameraClass::create
//

OtOrthographicCamera OtOrthographicCameraClass::create() {
	OtOrthographicCamera camera = std::make_shared<OtOrthographicCameraClass>();
	camera->setType(getMeta());
	return camera;
}
