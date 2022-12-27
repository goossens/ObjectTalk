//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include  <utility>

#include "entt/entity/registry.hpp"

#include "OtException.h"


//
//	OtEntity
//

class OtEntity {
public:
	// constructor
	OtEntity() = default;
	OtEntity(entt::registry* r, entt::entity e) : registry(r), entity(e) {}

	// add a new component to this entity
	template<typename T, typename... Args>
	T& addComponent(Args&&... args) {
		OT_ASSERT(!hasComponent<T>());
		return registry->emplace<T>(entity, std::forward<Args>(args)...);
	}

	// get a specified component
	template<typename T>
	T& getComponent() {
		OT_ASSERT(hasComponent<T>());
		return registry->get<T>(entity);
	}

	// remove a specified component
	template<typename T>
	bool removeComponent() {
		OT_ASSERT(hasComponent<T>());
		return registry->remove<T>(entity);
	}

	// see if we have the specified component
	template<typename T>
	bool hasComponent() {
		return registry->all_of<T>(entity);
	}

	// see if entity is valid
	bool isValid() { return entity != entt::null; }

	// conversions
	operator entt::entity() {
		return entity;
	}

	// see if entity is valid
	operator bool() const {
		return registry != nullptr && entity != entt::null;
	}

private:
	// registry and entity handle
	entt::registry* registry = nullptr;
	entt::entity entity = entt::null;
};
