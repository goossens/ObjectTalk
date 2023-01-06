//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <cstdint>
#include <unordered_map>

#include "entt/entity/registry.hpp"

#include "OtEntity.h"


//
//	OtScene2
//

class OtScene2 {
public:
	// constructor/destructor
	OtScene2();
	~OtScene2();

	// create a new entity
	OtEntity createEntity(const std::string& tag=std::string("Unknown"));

	// clone an entity
	OtEntity cloneEntity(OtEntity entity);

	// get an existing entity from an identifier
	OtEntity getEntity(entt::entity entity);
	OtEntity getEntity(uint64_t id);
	OtEntity getEntity(const std::string& tag);

	// see if entity exists based on identifier
	bool hasEntity(entt::entity entity);
	bool hasEntity(uint64_t id);
	bool hasEntity(const std::string& tag);

private:
	// scene identifier
	uint64_t id;

	// registry for all entities and components in ths scene
	entt::registry registry;

	// entity lookup by ID
	std::unordered_map<uint64_t, entt::entity> mapIdToEntity;
	std::unordered_map<entt::entity, uint64_t> mapEntityToId;
};
