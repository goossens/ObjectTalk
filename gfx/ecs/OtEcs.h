//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
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

	// clear entire system
	void clear();

	// create a new entity
	OtEntity createEntity(uint32_t uuid=0, const std::string& tag="untitled");

	// see if entity is valid
	inline bool isValidEntity(OtEntity entity) {
		return entity != OtEntityNull && registry.valid(entity);
	}

	// get an existing entity from an identifier
	OtEntity getEntity(const std::string& tag);

	// see if entity exists based on an identifier
	inline bool hasEntity(const std::string& tag) {
		return getEntity(tag) != OtEntityNull;
	}

	// remove entity
	void removeEntity(OtEntity entity);

	// move entity to a new parent
	void moveEntityBefore(OtEntity sibling, OtEntity child);
	void moveEntityTo(OtEntity parent, OtEntity child);

	// get root entity
	inline OtEntity getRootEntity() { return root; }

	// see if entity has children
	inline bool hasChildren(OtEntity entity) {
		return getComponent<OtCoreComponent>(entity).firstChild != OtEntityNull;
	}

	// get core information about entity
	inline uint32_t getUuid(OtEntity entity) { return getComponent<OtCoreComponent>(entity).uuid; }
	inline std::string getTag(OtEntity entity) { return getComponent<OtCoreComponent>(entity).tag; }
	inline OtEntity getParent(OtEntity entity) { return getComponent<OtCoreComponent>(entity).parent; }
	inline OtEntity getFirstChild(OtEntity entity) { return getComponent<OtCoreComponent>(entity).firstChild; }
	inline OtEntity getLastChild(OtEntity entity) { return getComponent<OtCoreComponent>(entity).lastChild; }
	inline OtEntity getPreviousSibling(OtEntity entity) { return getComponent<OtCoreComponent>(entity).previousSibling; }
	inline OtEntity getNextSibling(OtEntity entity) { return getComponent<OtCoreComponent>(entity).nextSibling; }

	// iterate through all entities (order is not guaranteed)
	inline void each(std::function<void(OtEntity)> callback) {
		for (auto [entity] : registry.storage<OtEntity>().each()) {
			// don't expose the hidden root entity
			if (entity != root) {
				callback(entity);
			}
		};
	}

	// iterate through an entity's children (just one level)
	inline void eachChild(OtEntity entity, std::function<void(OtEntity)> callback) {
		auto child = getComponent<OtCoreComponent>(entity).firstChild;

		while (isValidEntity(child)) {
			auto nextChild = getComponent<OtCoreComponent>(child).nextSibling;
			callback(child);
			child = nextChild;
		}
	}

	// iterate through all of an entity's children (depth-first)
	inline void eachChildDepthFirst(OtEntity entity, std::function<void(OtEntity)> callback) {
		auto child = getComponent<OtCoreComponent>(entity).firstChild;

		while (isValidEntity(child)) {
			auto nextChild = getComponent<OtCoreComponent>(child).nextSibling;
			callback(child);
			eachChild(child, callback);
			child = nextChild;
		}
	}

	// iterate through all entities in depth-first order (based on hierarchy)
	inline void eachEntityDepthFirst(std::function<void(OtEntity)> callback) { eachChildDepthFirst(root, callback); }

	// add a new component to an entity
	template<typename T, typename... Args>
	inline T& addComponent(OtEntity entity, Args&&... args) {
		return registry.emplace<T>(entity, std::forward<Args>(args)...);
	}

	// get a specified component from an entity
	template<typename T>
	inline T& getComponent(OtEntity entity) {
		return registry.get<T>(entity);
	}

	// get an specified component from and entity and add one if it doesn;t exist
	template<typename T>
	inline T& getOrAddComponent(OtEntity entity) {
		if (registry.all_of<T>(entity)) {
			return registry.get<T>(entity);

		} else {
			return registry.emplace<T>(entity);
		}
	}

	// remove a specified component
	template<typename T>
	inline bool removeComponent(OtEntity entity) {
		return registry.remove<T>(entity);
	}

	// see if we have the specified component
	template<typename T>
	inline bool hasComponent(OtEntity entity) {
		return registry.all_of<T>(entity);
	}

	// get a view
	template<typename... T>
	inline auto view() {
		return registry.view<T...>();
	}

	// translate entity <-> UUID
	inline uint32_t getUuidFromEntity(OtEntity entity) { return mapEntityToUuid[entity]; }
	inline OtEntity getEntityFromUuid(uint32_t uuid) { return mapUuidToEntity[uuid]; }

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
