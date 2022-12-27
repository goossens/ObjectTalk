//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>

#include "glm/glm.hpp"
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
//	OtTagComponent
//

struct OtTagComponent {
	// constructors
	OtTagComponent() = default;
	OtTagComponent(const std::string& t) : tag(t) {}

	// properties
	std::string tag;
};


//
//	OtHierarchyComponent
//

struct OtHierarchyComponent {
	// properties
	entt::entity parent{entt::null};
	entt::entity first{entt::null};
	entt::entity prev{entt::null};
	entt::entity next{entt::null};
};


//
//	OtTransformComponent
//

struct OtTransformComponent {
	// constructors
	OtTransformComponent() = default;
	OtTransformComponent(const glm::mat4& t) : transform(t) {}

	// properties
	glm::mat4 transform{1.0};
};
