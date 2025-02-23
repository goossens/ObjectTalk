//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/quaternion.hpp"
#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtGlm.h"
#include "OtTransformComponent.h"
#include "OtUi.h"


//
//	OtTransformComponent::getTransform
//

glm::mat4 OtTransformComponent::getTransform() {
	return glm::translate(glm::mat4(1.0f), translation) *
		glm::toMat4(glm::quat(glm::radians(rotation))) *
		glm::scale(glm::mat4(1.0f), scale);
}


//
//	OtTransformComponent::renderUI
//

bool OtTransformComponent::renderUI() {
	// render transformation editor
	bool changed = false;
	changed |= OtUi::editVec3("Translate", &translation);
	changed |= OtUi::editVec3("Rotate", &rotation);
	changed |= OtUi::editVec3("Scale", &scale);
	return changed;
}


//
//	OtTransformComponent::serialize
//

nlohmann::json OtTransformComponent::serialize(std::string* /* basedir */) {
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

void OtTransformComponent::deserialize(nlohmann::json data, std::string* /* basedir */) {
	translation = data.value("translation", glm::vec3(0.0f, 0.0f, 0.0f));
	rotation = data.value("rotation", glm::vec3(0.0f, 0.0f, 0.0f));
	scale = data.value("scale", glm::vec3(1.0f, 1.0f, 1.0f));
}
