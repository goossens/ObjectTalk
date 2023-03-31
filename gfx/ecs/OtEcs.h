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

#include "OtEntity.h"
#include "OtCoreComponent.h"


//
//	OtEcs (Entity Component System)
//

class OtEcs {
public:
	// constructor/destructor
	OtEcs();
	~OtEcs();

	// create a new entity
	OtEntity createEntity(uint32_t uuid=0, const std::string& tag="untitled");

	// see if entity is valid
	bool isValidEntity(OtEntity entity) {
		return entity != OtEntityNull && registry.valid(entity);
	}

	// get an existing entity from an identifier
	OtEntity getEntity(const std::string& tag);

	// see if entity exists based on an identifier
	bool hasEntity(const std::string& tag) {
		return getEntity(tag) != OtEntityNull;
	}

	// remove entity
	void removeEntity(OtEntity entity);

	// move entity to a new parent
	void moveEntityBefore(OtEntity sibling, OtEntity child);
	void moveEntityTo(OtEntity parent, OtEntity child);

	// get root entity
	OtEntity getRootEntity() { return root; }

	// see if entity has children
	bool hasChildren(OtEntity entity) {
		return getComponent<OtCoreComponent>(entity).firstChild != OtEntityNull;
	}

	// get core information about entity
	uint32_t getUuid(OtEntity entity) { return getComponent<OtCoreComponent>(entity).uuid; }
	std::string getTag(OtEntity entity) { return getComponent<OtCoreComponent>(entity).tag; }
	OtEntity getParent(OtEntity entity) { return getComponent<OtCoreComponent>(entity).parent; }
	OtEntity getFirstChild(OtEntity entity) { return getComponent<OtCoreComponent>(entity).firstChild; }
	OtEntity getLastChild(OtEntity entity) { return getComponent<OtCoreComponent>(entity).lastChild; }
	OtEntity getPreviousSibling(OtEntity entity) { return getComponent<OtCoreComponent>(entity).previousSibling; }
	OtEntity getNextSibling(OtEntity entity) { return getComponent<OtCoreComponent>(entity).nextSibling; }

	// iterate through all entities (order is not garuenteed)
	void each(std::function<void(OtEntity)> callback) {
		registry.each([this, callback](OtEntity entity) {
			// don't expose the hidden root entity
			if (entity != root) {
				callback(entity);
			}
		});
	}

	// iterate through an entity's children (just one level)
	void eachChild(OtEntity entity, std::function<void(OtEntity)> callback) {
		auto child = getComponent<OtCoreComponent>(entity).firstChild;

		while (isValidEntity(child)) {
			auto nextChild = getComponent<OtCoreComponent>(child).nextSibling;
			callback(child);
			child = nextChild;
		}
	}

	// iterate through all of an entity's children (depth-first)
	void eachChildDepthFirst(OtEntity entity, std::function<void(OtEntity)> callback) {
		auto child = getComponent<OtCoreComponent>(entity).firstChild;

		while (isValidEntity(child)) {
			auto nextChild = getComponent<OtCoreComponent>(child).nextSibling;
			callback(child);
			eachChild(child, callback);
			child = nextChild;
		}
	}

	// iterate through all entities in depth-first order (based on hierarchy)
	void eachEntityDepthFirst(std::function<void(OtEntity)> callback) { eachChildDepthFirst(root, callback); }

	// add a new component to an entity
	template<typename T, typename... Args>
	T& addComponent(OtEntity entity, Args&&... args) {
		return registry.emplace<T>(entity, std::forward<Args>(args)...);
	}

	// get a specified component from an entity
	template<typename T>
	T& getComponent(OtEntity entity) {
		return registry.get<T>(entity);
	}

	// get an specified component from and entity and add one if it doesn;t exist
	template<typename T>
	T& getOrAddComponent(OtEntity entity) {
		if (registry.all_of<T>(entity)) {
			return registry.get<T>(entity);

		} else {
			return registry.emplace<T>(entity);
		}
	}

	// remove a specified component
	template<typename T>
	bool removeComponent(OtEntity entity) {
		return registry.remove<T>(entity);
	}

	// see if we have the specified component
	template<typename T>
	bool hasComponent(OtEntity entity) {
		return registry.all_of<T>(entity);
	}

	// get a view
	template<typename... T>
	auto view() {
		return registry.view<T...>();
	}

	// translate entity <-> UUID
	uint32_t getUuidFromEntity(OtEntity entity) { return mapEntityToUuid[entity]; }
	OtEntity getEntityFromUuid(uint32_t uuid) { return mapUuidToEntity[uuid]; }

	// set/get an entity's UUID
	uint32_t getEntityUuid(OtEntity entity);
	void setEntityUuid(OtEntity entity, uint32_t uuid);

	// remap an entity's UUID
	void remapEntityUuid(OtEntity entity, uint32_t oldUuid, uint32_t newUuid);

	// assign new UUIDs to an entity and all its children
	void assignNewEntityUuids(OtEntity entity);

	// set/get an entity's tag
	std::string getEntityTag(OtEntity entity);
	void setEntityTag(OtEntity entity, const std::string& tag);

	// hierarchy support functions
	void addEntityToParent(OtEntity parent, OtEntity child);
	void insertEntityBefore(OtEntity sibling, OtEntity child);
	void insertEntityAfter(OtEntity sibling, OtEntity child);
	void removeEntityFromParent(OtEntity entity);

private:
	// registry for all entities and components
	entt::registry registry;

	// entity lookup by ID
	std::unordered_map<uint32_t, entt::entity> mapUuidToEntity;
	std::unordered_map<entt::entity, uint32_t> mapEntityToUuid;

	// hierarchy support
	OtEntity root;
};
