//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
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
//	OtEcs::createEntity
//

OtEntity OtEcs::createEntity() {
	// create a new entity
	auto entity = registry.create();

	// add default components
	auto uuid = addComponent<OtUuidComponent>(entity).uuid;
	addComponent<OtTagComponent>(entity).tag = "untitled";
	addComponent<OtHierarchyComponent>(entity);

	// register entity <-> UUID mappings
	mapUuidToEntity[uuid] = entity;
	mapEntityToUuid[entity] = uuid;
	return entity;
}

OtEntity OtEcs::createEntity(OtEntity parent) {
	// create a new entity
	auto entity = createEntity();

	// use the root entity is parent is invalid
	if (!isValidEntity(parent)) {
		parent = root;
	}

	// add entity to parent
	addEntityToParent(parent, entity);
	return entity;
}


//
//	OtEcs::getEntity
//

OtEntity OtEcs::getEntity(const std::string &tag) {
	auto view = registry.view<OtTagComponent>();

	for (auto entity : registry.view<OtTagComponent>()) {
		if (view.get<OtTagComponent>(entity).tag == tag) {
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
//	OtEcs::assignNewEntityUUids
//

void OtEcs::assignNewEntityUUids(OtEntity entity) {
	// remove old mappings
	mapUuidToEntity.erase(mapEntityToUuid[entity]);
	mapEntityToUuid.erase(entity);

	// assign new UUID to entity
	auto& component = getComponent<OtUuidComponent>(entity);
	component.assignNewUuid();

	// create new mappings
	auto uuid = component.uuid;
	mapUuidToEntity[uuid] = entity;
	mapEntityToUuid[entity] = uuid;

	// assign new UUIDs to entity and all its children
	eachChild(entity, [this](OtEntity child) {
		assignNewEntityUUids(child);
	});
}


//
//	OtEcs::addEntityToParent
//

void OtEcs::addEntityToParent(OtEntity parent, OtEntity child) {
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
//	OtEcs::insertEntityBefore
//

void OtEcs::insertEntityBefore(OtEntity sibling, OtEntity child) {
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
//	OtEcs::insertEntityAfter
//

void OtEcs::insertEntityAfter(OtEntity sibling, OtEntity child) {
	// get details on sibling, parent and child
	auto& siblingHierarchy = getComponent<OtHierarchyComponent>(sibling);
	auto& parentHierarchy = getComponent<OtHierarchyComponent>(siblingHierarchy.parent);
	auto& childHierarchy = getComponent<OtHierarchyComponent>(child);

	// parent the child
	childHierarchy.parent = siblingHierarchy.parent;

	// update the sibling chain
	if (parentHierarchy.lastChild == sibling) {
		parentHierarchy.lastChild = child;

	} else {
		getComponent<OtHierarchyComponent>(siblingHierarchy.nextSibling).previousSibling = child;
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
