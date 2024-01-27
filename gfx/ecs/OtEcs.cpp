//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtEcs.h"


//
//	OtEcs::OtEcs
//

OtEcs::OtEcs() {
	// create a hidden root entity to build hierarchies
	root = createEntity();
}


//
//	OtEcs::~OtEcs
//

OtEcs::~OtEcs() {
}


//
//	OtEcs::clear
//

void OtEcs::clear() {
	registry.clear();
	root = createEntity();
}


//
//	OtEcs::createEntity
//

OtEntity OtEcs::createEntity(uint32_t uuid, const std::string& tag) {
	// create a new entity
	auto entity = registry.create();

	// add core component
	auto newUuid = addComponent<OtCoreComponent>(entity, uuid, tag).uuid;

	// register entity <-> UUID mappings
	mapUuidToEntity[newUuid] = entity;
	mapEntityToUuid[entity] = newUuid;

	// return new entity
	return entity;
}


//
//	OtEcs::getEntity
//

OtEntity OtEcs::getEntity(const std::string& tag) {
	for (auto entity : registry.view<OtCoreComponent>()) {
		if (getComponent<OtCoreComponent>(entity).tag == tag) {
			return entity;
		}
	}

	return OtEntityNull;
}


//
//	OtEcs::removeEntity
//

void OtEcs::removeEntity(OtEntity entity) {
	// remove all children
	eachChild(entity, [this](OtEntity child) {
		removeEntity(child);
	});

	// remove entity from hierarchy
	removeEntityFromParent(entity);

	// remove entity from entity <-> UUID mappings
	mapUuidToEntity.erase(mapEntityToUuid[entity]);
	mapEntityToUuid.erase(entity);

	// remove entity from registry
	registry.destroy(entity);
}


//
//	OtEcs::moveEntityBefore
//

void OtEcs::moveEntityBefore(OtEntity sibling, OtEntity child) {
	// don't move before yourself
	if (sibling != child) {
		// remove entity from old parent
		removeEntityFromParent(child);
		insertEntityBefore(sibling, child);
	}
}


//
//	OtEcs::moveEntityTo
//

void OtEcs::moveEntityTo(OtEntity parent, OtEntity child) {
	removeEntityFromParent(child);
	addEntityToParent(parent, child);
}


//
//	OtEcs::getEntityUuid
//

uint32_t OtEcs::getEntityUuid(OtEntity entity) {
	return getComponent<OtCoreComponent>(entity).uuid;
}


//
//	OtEcs::setEntityUuid
//

void OtEcs::setEntityUuid(OtEntity entity, uint32_t uuid) {
	// get the core component
	auto& component = getComponent<OtCoreComponent>(entity);

	// remove old mappings
	mapUuidToEntity.erase(component.uuid);
	mapEntityToUuid.erase(entity);

	// create new mappings
	mapUuidToEntity[uuid] = entity;
	mapEntityToUuid[entity] = uuid;

	// update the component
	component.uuid = uuid;
}


//
//	OtEcs::remapEntityUuid
//

void OtEcs::remapEntityUuid(OtEntity entity, uint32_t oldUuid, uint32_t newUuid) {
	// remove old mappings
	mapUuidToEntity.erase(oldUuid);
	mapEntityToUuid.erase(entity);

	// create new mappings
	mapUuidToEntity[newUuid] = entity;
	mapEntityToUuid[entity] = newUuid;
}


//
//	OtEcs::assignNewEntityUuids
//

void OtEcs::assignNewEntityUuids(OtEntity entity) {
	// remove old mappings
	mapUuidToEntity.erase(mapEntityToUuid[entity]);
	mapEntityToUuid.erase(entity);

	// assign new UUID to entity
	auto& component = getComponent<OtCoreComponent>(entity);
	component.assignNewUuid();

	// create new mappings
	auto uuid = component.uuid;
	mapUuidToEntity[uuid] = entity;
	mapEntityToUuid[entity] = uuid;

	// assign new UUIDs to all its children
	eachChild(entity, [this](OtEntity child) {
		assignNewEntityUuids(child);
	});
}


//
//	OtEcs::getEntityTag
//

std::string OtEcs::getEntityTag(OtEntity entity) {
	return getComponent<OtCoreComponent>(entity).tag;
}


//
//	OtEcs::setEntityTag
//

void OtEcs::setEntityTag(OtEntity entity, const std::string& tag) {
	auto& component = getComponent<OtCoreComponent>(entity);
	component.tag = tag;
}


//
//	OtEcs::addEntityToParent
//

void OtEcs::addEntityToParent(OtEntity parent, OtEntity child) {
	// get details on parent and child
	auto& parentHierarchy = getComponent<OtCoreComponent>(parent);
	auto& childHierarchy = getComponent<OtCoreComponent>(child);

	// sort out the child's siblings (child will added to the end of the parent's children)
	childHierarchy.previousSibling = parentHierarchy.lastChild;
	childHierarchy.nextSibling = OtEntityNull;

	// update new parent
	childHierarchy.parent = parent;

	// does parent have any kids?
	if (isValidEntity(parentHierarchy.firstChild)) {
		// yes, add child to the end
		registry.get<OtCoreComponent>(parentHierarchy.lastChild).nextSibling = child;

	} else {
		// no, this is now the parent's only child
		parentHierarchy.firstChild = child;
	}

	// either way, this is now the last child
	parentHierarchy.lastChild = child;
}


//
//	OtEcs::insertEntityBefore
//

void OtEcs::insertEntityBefore(OtEntity sibling, OtEntity child) {
	// get details on sibling, parent and child
	auto& siblingHierarchy = getComponent<OtCoreComponent>(sibling);
	auto& parentHierarchy = getComponent<OtCoreComponent>(siblingHierarchy.parent);
	auto& childHierarchy = getComponent<OtCoreComponent>(child);

	// parent the child
	childHierarchy.parent = siblingHierarchy.parent;

	// update the sibling chain
	if (parentHierarchy.firstChild == sibling) {
		parentHierarchy.firstChild = child;

	} else {
		getComponent<OtCoreComponent>(siblingHierarchy.previousSibling).nextSibling = child;
	}

	childHierarchy.previousSibling = siblingHierarchy.previousSibling;
	childHierarchy.nextSibling = sibling;
	siblingHierarchy.previousSibling = child;
}


//
//	OtEcs::insertEntityAfter
//

void OtEcs::insertEntityAfter(OtEntity sibling, OtEntity child) {
	// get details on sibling, parent and child
	auto& siblingHierarchy = getComponent<OtCoreComponent>(sibling);
	auto& parentHierarchy = getComponent<OtCoreComponent>(siblingHierarchy.parent);
	auto& childHierarchy = getComponent<OtCoreComponent>(child);

	// parent the child
	childHierarchy.parent = siblingHierarchy.parent;

	// update the sibling chain
	if (parentHierarchy.lastChild == sibling) {
		parentHierarchy.lastChild = child;

	} else {
		getComponent<OtCoreComponent>(siblingHierarchy.nextSibling).previousSibling = child;
	}

	childHierarchy.previousSibling = sibling;
	childHierarchy.nextSibling = siblingHierarchy.nextSibling;
	siblingHierarchy.nextSibling = child;
}


//
//	OtEcs::removeEntityFromParent
//

void OtEcs::removeEntityFromParent(OtEntity entity) {
	// get details on entity and parent
	auto& entityHierarchy = getComponent<OtCoreComponent>(entity);

	// don't worry if the entity has no parent
	if (isValidEntity(entityHierarchy.parent)) {
		auto& parentHierarchy = getComponent<OtCoreComponent>(entityHierarchy.parent);

		// disconnect from parent
		if (parentHierarchy.firstChild == entity) {
			parentHierarchy.firstChild = entityHierarchy.nextSibling;
		}

		if (parentHierarchy.lastChild == entity) {
			parentHierarchy.lastChild = entityHierarchy.previousSibling;
		}

		// disconnect from siblings
		if (isValidEntity(entityHierarchy.previousSibling)) {
			auto& siblingHierarchy = getComponent<OtCoreComponent>(entityHierarchy.previousSibling);
			siblingHierarchy.nextSibling = entityHierarchy.nextSibling;
		}

		if (isValidEntity(entityHierarchy.nextSibling)) {
			auto& siblingHierarchy = getComponent<OtCoreComponent>(entityHierarchy.nextSibling);
			siblingHierarchy.previousSibling = entityHierarchy.previousSibling;
		}

		// let the entity dangle (caller must reparent enitty)
		entityHierarchy.parent = OtEntityNull;
		entityHierarchy.previousSibling = OtEntityNull;
		entityHierarchy.nextSibling = OtEntityNull;
	}
}
