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

#include "OtComponents.h"
#include "OtEntity.h"


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
	OtEntity createEntity();
	OtEntity createEntity(OtEntity parent);

	// see if entity is valid
	bool isValidEntity(OtEntity entity) {
		return registry.valid(entity);
	}

	// get an existing entity from an identifier
	OtEntity getEntity(const std::string& name);

	// see if entity exists based on an identifier
	bool hasEntity(const std::string& name) {
		return isValidEntity(getEntity(name));
	}

	// remove entity
	void removeEntity(OtEntity entity);

	// move entity to a new parent
	void moveEntityBefore(OtEntity sibling, OtEntity child);
	void moveEntityTo(OtEntity parent, OtEntity child);

	// get root entity in scene
	OtEntity getRootEntity() { return root; }

	// see if entity has children
	bool hasChildren(OtEntity entity) {
		return getComponent<OtHierarchyComponent>(entity).firstChild != OtEntityNull;
	}

	// access neighbors in parent-child hierarchy
	OtEntity getParent(OtEntity entity) { return getComponent<OtHierarchyComponent>(entity).parent; }
	OtEntity getFirstChild(OtEntity entity) { return getComponent<OtHierarchyComponent>(entity).firstChild; }
	OtEntity getLastChild(OtEntity entity) { return getComponent<OtHierarchyComponent>(entity).lastChild; }
	OtEntity getPreviousSibling(OtEntity entity) { return getComponent<OtHierarchyComponent>(entity).previousSibling; }
	OtEntity getNextSibling(OtEntity entity) { return getComponent<OtHierarchyComponent>(entity).nextSibling; }

	// iterate through entities in scene
	void each(std::function<void(OtEntity)> callback) {
		registry.each([this, callback](OtEntity entity) {
			// don't expose the hidden root entity
			if (entity != root) {
				callback(entity);
			}
		});
	}

	// iterate through an entity's children
	void eachChild(OtEntity entity, std::function<void(OtEntity)> callback) {
		auto child = getComponent<OtHierarchyComponent>(entity).firstChild;

		while (isValidEntity(child)) {
			auto nextChild = getComponent<OtHierarchyComponent>(child).nextSibling;
			callback(child);
			child = nextChild;
		}
	}

	// get the ID of a component type
	template<typename T>
	uint32_t getComponentTypeId() {
		return entt::type_id<T>().index();
	}

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

	// translate entity <-> UUID
	uint32_t getUuidFromEntity(OtEntity entity) { return mapEntityToUuid[entity]; }
	OtEntity getEntityFromUuid(uint32_t uuid) { return mapUuidToEntity[uuid]; }

	// change an entity's UUID and update translation tables
	void updateEntityUuid(OtEntity entity, uint32_t uuid);

	// hierarchy support functions
	void addEntityToParent(OtEntity parent, OtEntity child);
	void insertEntityBefore(OtEntity sibling, OtEntity child);
	void insertEntityAfter(OtEntity sibling, OtEntity child);
	void removeEntityFromParent(OtEntity entity);

	// (de)serialize the scene
	std::string serialize(int indent=-1, char character=' ');
	void deserialize(const std::string& data, bool preserveUuid=true);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtScene2 create();

private:
	// registry for all entities and components in this scene
	entt::registry registry;

	// entity lookup by ID
	std::unordered_map<uint32_t, entt::entity> mapUuidToEntity;
	std::unordered_map<entt::entity, uint32_t> mapEntityToUuid;

	// scene hierarchy support
	OtEntity root;
};
