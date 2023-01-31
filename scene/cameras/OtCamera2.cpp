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

#include "OtCamera2.h"


//
//	OtCamera2Class::updateViewMatrix
//

void OtCamera2Class::updateViewMatrix() {
	viewMatrix = glm::lookAt(position, target, up);
	OtCamera2Class::updateDerived();
}


//
//	OtCamera2Class::updateDerived
//

void OtCamera2Class::updateDerived() {
	// derived classes are responsible for setting the projection matrix
	viewProjMatrix = projMatrix * viewMatrix;
	frustum = OtFrustum(viewProjMatrix);
}


//
//	OtCamera2Class::renderGUI
//

bool OtCamera2Class::renderGUI() {
	bool changed = false;
	changed |= ImGui::InputFloat3("Position", glm::value_ptr(position));
	changed |= ImGui::InputFloat3("Target", glm::value_ptr(target));
	changed |= ImGui::SliderFloat3("Up", glm::value_ptr(up), -1.0f, 1.0f);

	if (changed) {
		updateViewMatrix();
	}

	return changed;
}


//
//	OtCamera2Class::serialize
//

nlohmann::json OtCamera2Class::serialize() {
	auto data = nlohmann::json::object();
	data["type"] = name;
	data["position"] = position;
	data["target"] = target;
	data["up"] = up;
	return data;
}


//
//	OtCamera2Class::deserialize
//

void OtCamera2Class::deserialize(nlohmann::json data) {
	position = data["position"];
	target = data["target"];
	up = data["up"];

	updateViewMatrix();
}


//
//	OtCamera2Class::serializeToString
//

std::string OtCamera2Class::serializeToString() {
	 return serialize().dump();
}


//
//	OtCamera2Class::deserializeFromString
//

void OtCamera2Class::deserializeFromString(const std::string& data) {
	return deserialize(nlohmann::json::parse(data));
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
