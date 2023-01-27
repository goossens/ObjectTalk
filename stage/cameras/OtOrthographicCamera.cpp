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

#include "OtGpu.h"
#include "OtOrthographicCamera.h"


//
//	OtOrthographicCameraClass::setWidth
//

void OtOrthographicCameraClass::setWidth(float w) {
	width = w;
	auto height = width / aspectRatio;

	left = -width / 2.0;
	right = width / 2.0;
	top = height / 2.0;
	bottom = -height / 2.0;

	updateProjectionMatrix();
}


//
//	OtOrthographicCameraClass::setAspectRatio
//

void OtOrthographicCameraClass::setAspectRatio(float ar) {
	aspectRatio = ar;
	auto height = width / aspectRatio;

	top = height / 2.0;
	bottom = -height / 2.0;

	updateProjectionMatrix();
}


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

	width = right - left;
	aspectRatio = width / (top - bottom);

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

	changed |= ImGui::DragFloat("Width", &width, 1.0, 0.0, 0.0, ".0f");
	changed |= ImGui::DragFloat("Near Plane", &near, 1.0, 0.0, 0.0, ".0f");
	changed |= ImGui::DragFloat("Far Plane", &far, 1.0, 0.0, 0.0, ".0f");

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
	return data;
}


//
//	OtOrthographicCameraClass::deserialize
//

void OtOrthographicCameraClass::deserialize(nlohmann::json data) {
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
