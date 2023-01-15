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
#include <functional>
#include <unordered_map>

#include "entt/entity/registry.hpp"

#include "OtGui.h"

#include "OtEntity.h"
#include "OtComponents.h"


//
//	OtScene2
//

class OtScene2Class;
typedef std::shared_ptr<OtScene2Class> OtScene2;

class OtScene2Class : public OtGuiClass {
public:
	// constructor/destructor
	OtScene2Class();
	~OtScene2Class();

	// create a new entity
	OtEntity createEntity(const std::string& tag=std::string("untitled"));

	// clone an entity
	OtEntity cloneEntity(OtEntity entity);

	// get an existing entity from an identifier
	OtEntity getEntity(entt::entity entity);
	OtEntity getEntity(uint64_t id);
	OtEntity getEntity(const std::string& name);

	// see if entity exists based on an identifier
	bool hasEntity(entt::entity entity);
	bool hasEntity(uint64_t id);
	bool hasEntity(const std::string& name);

	// remove entity
	void removeEntity(OtEntity entity);

	// iterate through entities in scene
	void each(std::function<void(OtEntity)> callback) {
		registry.each([this, callback](entt::entity entity) {
			callback(getEntity(entity));
		});
	}

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtScene2 create();

private:
	// scene identifier
	uint64_t id;

	// registry for all entities and components in this scene
	entt::registry registry;

	// entity mapping (entity <-> unique ID)
	std::unordered_map<uint64_t, entt::entity> mapIdToEntity;
	std::unordered_map<entt::entity, uint64_t> mapEntityToId;
};
