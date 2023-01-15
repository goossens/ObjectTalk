//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>

#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include "entt/entity/registry.hpp"


//
//	OtIdComponent
//

struct OtIdComponent {
	// constructors
	OtIdComponent(uint64_t i) : id(i) {}

	// properties
	uint64_t id;
};


//
//	OtNameComponent
//

struct OtNameComponent {
	// constructors
	OtNameComponent() = default;
	OtNameComponent(const std::string& n) : name(n) {}

	// properties
	std::string name;
};


//
//	OtTransformComponent
//

struct OtTransformComponent {
	// constructors
	OtTransformComponent() = default;

	OtTransformComponent(const glm::vec3& t, const glm::vec3& r, const glm::vec3& s) :
		translation(t), rotation(r), scale(s), active(true) {}

	// get the full transform
	glm::mat4 getTransform() const {
		return glm::translate(glm::mat4(1.0f), translation) *
			glm::toMat4(glm::quat(rotation)) *
			glm::scale(glm::mat4(1.0f), scale);
	}

	// reset the transform
	void reset() {
		translation = { 0.0f, 0.0f, 0.0f };
		rotation = { 0.0f, 0.0f, 0.0f };
		scale = { 1.0f, 1.0f, 1.0f };
	}

	// properties
	glm::vec3 translation = { 0.0f, 0.0f, 0.0f };
	glm::vec3 rotation = { 0.0f, 0.0f, 0.0f };
	glm::vec3 scale = { 1.0f, 1.0f, 1.0f };
	bool active = true;
};


//
//	OtGeometryComponent
//

struct OtGeometryComponent {
	// constructors
	OtGeometryComponent() = default;

	// properties
	bool active = true;
};
