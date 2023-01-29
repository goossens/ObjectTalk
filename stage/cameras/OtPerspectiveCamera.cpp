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
#include "OtPerspectiveCamera.h"


//
//	OtPerspectiveCameraClass::updateProjectionMatrix
//

void OtPerspectiveCameraClass::updateProjectionMatrix() {
	projMatrix = OtGpuHasHomogeneousDepth()
		? glm::perspectiveRH_NO(glm::radians(fov), aspectRatio, nearPlane, farPlane)
		: glm::perspectiveRH_ZO(glm::radians(fov), aspectRatio, nearPlane, farPlane);

	OtCamera2Class::updateDerived();
}


//
//	OtPerspectiveCameraClass::renderGUI
//

bool OtPerspectiveCameraClass::renderGUI() {
	bool changed = OtCamera2Class::renderGUI();

	changed |= ImGui::DragFloat("FoV (Deg)", &fov, 1.0f, 10.0f, 160.0f, "%.0f");
	changed |= ImGui::DragFloat("Near Plane", &nearPlane, 1.0f, 0.0f, 0.0f, "%.1f");
	changed |= ImGui::DragFloat("Far Plane", &farPlane, 1.0f, 0.0f, 0.0f, "%.1f");

	if (changed) {
		updateProjectionMatrix();
	}

	return changed;
}


//
//	OtPerspectiveCameraClass::getMeta
//

OtType OtPerspectiveCameraClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtPerspectiveCameraClass>("PerspectiveCamera", OtCamera2Class::getMeta());
	}

	return type;
}


//
//	OtPerspectiveCameraClass::serialize
//

nlohmann::json OtPerspectiveCameraClass::serialize() {
	auto data = nlohmann::json::object();
	data["type"] = name;

	data["position"] = position;
	data["target"] = target;
	data["up"] = up;

	data["fov"] = fov;
	data["near"] = nearPlane;
	data["far"] = farPlane;

	return data;
}


//
//	OtPerspectiveCameraClass::deserialize
//

void OtPerspectiveCameraClass::deserialize(nlohmann::json data) {
	position = data["position"];
	target = data["target"];
	up = data["up"];

	fov = data["fov"];
	nearPlane = data["near"];
	farPlane = data["far"];

	updateViewMatrix();
	updateProjectionMatrix();
}


//
//	OtPerspectiveCameraClass::create
//

OtPerspectiveCamera OtPerspectiveCameraClass::create() {
	OtPerspectiveCamera camera = std::make_shared<OtPerspectiveCameraClass>();
	camera->setType(getMeta());
	return camera;
}
