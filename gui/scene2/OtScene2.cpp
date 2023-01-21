//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <random>

#include "nlohmann/json.hpp"

#include "OtComponents.h"
#include "OtScene2.h"


//
//	createID
//

static uint32_t createUuid() {
	static std::random_device device;
	static std::mt19937_64 engine(device());
	static std::uniform_int_distribution<uint32_t> distribution;
	return distribution(engine);
}


//
//	OtScene2Class::OtScene2Class
//

OtScene2Class::OtScene2Class() {
	// create a hidden root entity to build hierarchies
	root = createEntity();
}


//
//	OtScene2::~OtScene2
//

OtScene2Class::~OtScene2Class() {
}


//
//	OtScene2Class::createEntity
//

OtEntity OtScene2Class::createEntity() {
	// create a new entity
	auto entity = registry.create();
	auto uuid = createUuid();

	// register mappings
	mapUuidToEntity[uuid] = entity;
	mapEntityToUuid[entity] = uuid;

	// add default components
	addComponent<OtUuidComponent>(entity).uuid = uuid;
	addComponent<OtNameComponent>(entity).name = "untitled";
	addComponent<OtHierarchyComponent>(entity);
	return entity;
}

OtEntity OtScene2Class::createEntity(OtEntity parent) {
	// create a new entity
	auto entity = createEntity();

	// setup the entity hierarchy
	if (!isValidEntity(parent)) {
		parent = root;
	}

	// add entity to parent
	addEntityToParent(parent, entity);
	return entity;
}


//
//	OtScene2Class::cloneEntity
//

template<typename T>
static void cloneComponentIfExists(OtEntity dst, OtEntity src, entt::registry& registry) {
	if (registry.all_of<T>(src)) {
		registry.emplace_or_replace<T>(dst, registry.get<T>(src));
	}
}

OtEntity OtScene2Class::cloneEntity(OtEntity entity) {
	OtEntity newEntity = createEntity();
	cloneComponentIfExists<OtNameComponent>(newEntity, entity, registry);
	cloneComponentIfExists<OtTransformComponent>(newEntity, entity, registry);
	return newEntity;
}


//
//	OtScene2::getEntity
//

OtEntity OtScene2Class::getEntity(const std::string &name) {
	auto view = registry.view<OtNameComponent>();

	for (auto entity : registry.view<OtNameComponent>()) {
		if (view.get<OtNameComponent>(entity).name == name) {
			return entity;
		}
	}

	return OtEntityNull;
}


//
//	OtScene2Class::removeEntity
//

void OtScene2Class::removeEntity(OtEntity entity) {
	// remove all children
	eachChild(entity, [this](OtEntity child) {
		removeEntity(child);
	});

	// remove entity from mappings
	mapUuidToEntity.erase(mapEntityToUuid[entity]);
	mapEntityToUuid.erase(entity);

	// remove entity from hierarchy
	removeEntityFromParent(entity);

	// remove entity from registry
	registry.destroy(entity);
}


//
//	OtScene2Class::moveEntityBefore
//

void OtScene2Class::moveEntityBefore(OtEntity sibling, OtEntity child) {
	// don't move before yourself
	if (sibling != child) {
		// remove entity from old parent
		removeEntityFromParent(child);
		insertEntityBefore(sibling, child);
	}
}


//
//	OtScene2Class::moveEntityTo
//

void OtScene2Class::moveEntityTo(OtEntity parent, OtEntity child) {
	removeEntityFromParent(child);
	addEntityToParent(parent, child);
}


//
//	OtScene2Class::serialize
//

nlohmann::json OtScene2Class::serialize() {
	// write entities and components
	auto data = nlohmann::json::array();

	eachChild(getRootEntity(), [&](OtEntity entity) {
		data.push_back(OtEntitySerialize(cast<OtScene2Class>(), entity));
	});

	return data;
}


//
//	OtScene2Class::deserialize
//

void OtScene2Class::deserialize(nlohmann::json data) {
	for (auto& entity : data) {
		addEntityToParent(root, OtEntityDeserialize(cast<OtScene2Class>(), entity));
	}
}


//
//	OtScene2Class::updateEntityUuid
//

void OtScene2Class::updateEntityUuid(OtEntity entity, uint32_t uuid) {
	// remove old mappings
	mapUuidToEntity.erase(mapEntityToUuid[entity]);
	mapEntityToUuid.erase(entity);

	// store new UUID
	getComponent<OtUuidComponent>(entity).uuid = uuid;

	// create new mappings
	mapUuidToEntity[uuid] = entity;
	mapEntityToUuid[entity] = uuid;
}


//
//	OtScene2Class::addEntityToParent
//

void OtScene2Class::addEntityToParent(OtEntity parent, OtEntity child) {
	// get details on parent and child
	auto& parentHierarchy = getComponent<OtHierarchyComponent>(parent);
	auto& childHierarchy = getComponent<OtHierarchyComponent>(child);

	// sort out the child's siblings (child will added to the end of the parent's children)
	childHierarchy.previousSibling = parentHierarchy.lastChild;
	childHierarchy.nextSibling = OtEntityNull;

	// update new parent
	childHierarchy.parent = parent;

	// does parent have any kids?
	if (isValidEntity(parentHierarchy.firstChild)) {
		// yes, add child to the end
		registry.get<OtHierarchyComponent>(parentHierarchy.lastChild).nextSibling = child;

	} else {
		// no, this is now the parent's only child
		parentHierarchy.firstChild = child;
	}

	// either way, this is now the last child
	parentHierarchy.lastChild = child;
}

//
//	OtScene2Class::insertEntityBefore
//

void OtScene2Class::insertEntityBefore(OtEntity sibling, OtEntity child) {
	// get details on sibling, parent and child
	auto& siblingHierarchy = getComponent<OtHierarchyComponent>(sibling);
	auto& parentHierarchy = getComponent<OtHierarchyComponent>(siblingHierarchy.parent);
	auto& childHierarchy = getComponent<OtHierarchyComponent>(child);

	// parent the child
	childHierarchy.parent = siblingHierarchy.parent;

	// update the sibling chain
	if (parentHierarchy.firstChild == sibling) {
		parentHierarchy.firstChild = child;

	} else {
		getComponent<OtHierarchyComponent>(siblingHierarchy.previousSibling).nextSibling = child;
	}

	childHierarchy.previousSibling = siblingHierarchy.previousSibling;
	childHierarchy.nextSibling = sibling;

	siblingHierarchy.previousSibling = child;
}


//
//	OtScene2Class::removeEntityFromParent
//

void OtScene2Class::removeEntityFromParent(OtEntity entity) {
	// get details on entity and parent
	auto& entityHierarchy = getComponent<OtHierarchyComponent>(entity);

	// don't worry if the entity has no parent
	if (!OtEntityIsNull(entityHierarchy.parent)) {
		auto& parentHierarchy = getComponent<OtHierarchyComponent>(entityHierarchy.parent);

		// disconnect from parent
		if (parentHierarchy.firstChild == entity) {
			parentHierarchy.firstChild = entityHierarchy.nextSibling;
		}

		if (parentHierarchy.lastChild == entity) {
			parentHierarchy.lastChild = entityHierarchy.previousSibling;
		}

		// disconnect from siblings
		if (isValidEntity(entityHierarchy.previousSibling)) {
			auto& siblingHierarchy = getComponent<OtHierarchyComponent>(entityHierarchy.previousSibling);
			siblingHierarchy.nextSibling = entityHierarchy.nextSibling;
		}

		if (isValidEntity(entityHierarchy.nextSibling)) {
			auto& siblingHierarchy = getComponent<OtHierarchyComponent>(entityHierarchy.nextSibling);
			siblingHierarchy.previousSibling = entityHierarchy.previousSibling;
		}

		// let the entity dangle (caller must reparent enitty)
		entityHierarchy.parent = OtEntityNull;
		entityHierarchy.previousSibling = OtEntityNull;
		entityHierarchy.nextSibling = OtEntityNull;
	}
}


//
//	OtScene2Class::getMeta
//

OtType OtScene2Class::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtScene2Class>("Scene2", OtGuiClass::getMeta());
	}

	return type;
}


//
//	OtScene2Class::create
//

OtScene2 OtScene2Class::create() {
	OtScene2 scene = std::make_shared<OtScene2Class>();
	scene->setType(getMeta());
	return scene;
}
