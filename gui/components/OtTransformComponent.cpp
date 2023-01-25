//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtGlm.h"
#include "OtTransformComponent.h"
#include "OtUi.h"


//
//	OtTransformComponent::getTransform
//

glm::mat4 OtTransformComponent::getTransform() const
{
	return glm::translate(glm::mat4(1.0f), translation) *
		glm::toMat4(glm::quat(rotation)) *
		glm::scale(glm::mat4(1.0f), scale);
}


//
//	OtTransformComponent::reset
//

void OtTransformComponent::reset() {
	translation = { 0.0f, 0.0f, 0.0f };
	rotation = { 0.0f, 0.0f, 0.0f };
	scale = { 1.0f, 1.0f, 1.0f };
}


//
//	OtTransformComponent::renderGUI
//

bool OtTransformComponent::renderGUI() {
	// render transformation matrix
	bool changed = false;
	changed |= OtUiEditVec3("Translate", translation, 0.1f, 0.0f, 0.0f, "%.2f");
	glm::vec3 rot = glm::degrees(rotation);
	changed |= OtUiEditVec3("Rotate", rot, 0.1f, 0.0f, 0.0f, "%.2f");
	rotation = glm::radians(rot);
	changed |= OtUiEditVec3("Scale", scale, 0.1f, 0.0f, 0.0f, "%.2f");
	return changed;
}


//
//	OtTransformComponent::serialize
//

nlohmann::json OtTransformComponent::serialize() {
	auto data = nlohmann::json::object();
	data["component"] = name;
	data["translation"] = translation;
	data["rotation"] = rotation;
	data["scale"] = scale;
	return data;
}


//
//	OtTransformComponent::deserialize
//

void OtTransformComponent::deserialize(nlohmann::json data) {
	translation = data["translation"];
	rotation = data["rotation"];
	scale = data["scale"];
}
