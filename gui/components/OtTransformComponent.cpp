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
#include "glm/ext.hpp"
#include "imgui.h"
#include "nlohmann/json.hpp"

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

void OtTransformComponent::renderGUI() {
	// render transformation matrix
	OtUiDragFloat("Translate", glm::value_ptr(translation), 3, 0.1f, 0.0f, 0.0f, "%.2f");
	glm::vec3 rot = glm::degrees(rotation);
	OtUiDragFloat("Rotate", glm::value_ptr(rot), 3, 0.1f, 0.0f, 0.0f, "%.2f");
	rotation = glm::radians(rot);
	OtUiDragFloat("Scale", glm::value_ptr(scale), 3, 0.1f, 0.0f, 0.0f, "%.2f");
}


//
//	OtTransformComponent::serialize
//

nlohmann::json OtTransformComponent::serialize() {
	auto data = nlohmann::json::object();
	data["type"] = "transform";
	data["translation"] = { translation.x, translation.y, translation.z };
	data["rotation"] = { rotation.x, rotation.y, rotation.z };
	data["scale"] = { scale.x, scale.y, scale.z };
	return data;
}


//
//	OtTransformComponent::deserialize
//

void OtTransformComponent::deserialize(nlohmann::json data) {
	translation.x = data["translation"][0];
	translation.y = data["translation"][1];
	translation.z = data["translation"][2];

	rotation.x = data["rotation"][0];
	rotation.y = data["rotation"][1];
	rotation.z = data["rotation"][2];

	scale.x = data["scale"][0];
	scale.y = data["scale"][1];
	scale.z = data["scale"][2];
}
