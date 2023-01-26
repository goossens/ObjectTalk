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
#include "OtPerspectiveCamera.h"


//
//	OtPerspectiveCameraClass::updateProjectionMatrix
//

void OtPerspectiveCameraClass::updateProjectionMatrix() {
	projMatrix = OtGpuHasHomogeneousDepth()
		? glm::perspectiveRH_NO(glm::radians(fov), aspectRatio, near, far)
		: glm::perspectiveRH_ZO(glm::radians(fov), aspectRatio, near, far);

	OtCamera2Class::updateDerived();
}


//
//	OtPerspectiveCameraClass::renderGUI
//

bool OtPerspectiveCameraClass::renderGUI() {
	bool changed = OtCamera2Class::renderGUI();

	changed |= ImGui::SliderFloat("FoV (Deg)", &fov, 10, 160);
	changed |= ImGui::DragFloat("Near Clipping", &near, 1.0, 0.0, 0.0, ".0f");
	changed |= ImGui::DragFloat("Far Clipping", &far, 1.0, 0.0, 0.0, ".0f");

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
	return data;
}


//
//	OtPerspectiveCameraClass::deserialize
//

void OtPerspectiveCameraClass::deserialize(nlohmann::json data) {
}


//
//	OtPerspectiveCameraClass::create
//

OtPerspectiveCamera OtPerspectiveCameraClass::create() {
	OtPerspectiveCamera camera = std::make_shared<OtPerspectiveCameraClass>();
	camera->setType(getMeta());
	return camera;
}
