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

#include "OtCamera2.h"


//
//	OtCamera2Class::updateViewMatrix
//

void OtCamera2Class::updateViewMatrix() {
	viewMatrix = glm::lookAt(position, target, up);
}


//
//	OtCamera2Class::updateDerived
//

void OtCamera2Class::updateDerived() {
	// derived classes are responsible for setting the projection and view matrices
	viewProjMatrix = projMatrix * viewMatrix;
	frustum = OtFrustum(viewProjMatrix);
}


//
//	OtCamera2Class::renderGUI
//

void OtCamera2Class::renderGUI() {
	bool changed = false;

	if (ImGui::InputFloat3("Position", glm::value_ptr(position))) { changed = true; }
	if (ImGui::InputFloat3("Target", glm::value_ptr(target))) { changed = true; }
	if (ImGui::SliderFloat3("Up", glm::value_ptr(up), -1.0f, 1.0f)) { changed = true; }

	if (changed) {
		updateViewMatrix();
	}
}


//
//	OtCamera2Class::getMeta
//

OtType OtCamera2Class::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtCamera2Class>("Camera2", OtGuiClass::getMeta());
	}

	return type;
}
