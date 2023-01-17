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

void OtPerspectiveCameraClass::renderGUI() {
	OtCamera2Class::renderGUI();

	bool changed = false;

	if (ImGui::SliderFloat("FoV (Deg)", &fov, 10, 160)) { changed = true; }
	if (ImGui::DragFloat("Near Clipping", &near, 1.0, 0.0, 0.0, ".0f")) { changed = true; }
	if (ImGui::DragFloat("Far Clipping", &far, 1.0, 0.0, 0.0, ".0f")) { changed = true; }

	if (changed) {
		updateProjectionMatrix();
	}
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
//	OtPerspectiveCameraClass::create
//

OtPerspectiveCamera OtPerspectiveCameraClass::create() {
	OtPerspectiveCamera camera = std::make_shared<OtPerspectiveCameraClass>();
	camera->setType(getMeta());
	return camera;
}
