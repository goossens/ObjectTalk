//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <random>

#include "OtComponents.h"
#include "OtScene2.h"


//
//	OtScene2::OtScene2
//

OtScene2::OtScene2() {
}


//
//	OtScene2::~OtScene2
//

OtScene2::~OtScene2() {
}


//
//	createID
//

static uint64_t createID() {
	static std::random_device device;
	static std::mt19937_64 engine(device());
	static std::uniform_int_distribution<uint64_t> distribution;
	return distribution(engine);
}


//
//	OtScene2::createEntity
//

OtEntity OtScene2::createEntity(const std::string& tag) {
	auto entity = OtEntity(&registry, registry.create());
	auto entityId = createID();
	entity.addComponent<OtIdComponent>(entityId);
	entity.addComponent<OtTagComponent>(tag);
	entity.addComponent<OtHierarchyComponent>();
	entity.addComponent<OtTransformComponent>();
	mapIdToEntity[entityId] = entity;
	mapEntityToId[entity] = entityId;
	return entity;
}


//
//	OtScene2::cloneEntity
//

template<typename T>
static void cloneComponentIfExists(OtEntity dst, OtEntity src, entt::registry& registry) {
	if (registry.all_of<T>(src)) {
		registry.emplace_or_replace<T>(dst, registry.get<T>(src));
	}
}

OtEntity OtScene2::cloneEntity(OtEntity entity) {
	OtEntity newEntity;

	if (entity.hasComponent<OtTagComponent>()) {
		newEntity = createEntity(entity.getComponent<OtTagComponent>().tag);

	} else {
		newEntity = createEntity();
	}

	cloneComponentIfExists<OtTagComponent>(newEntity, entity, registry);
	cloneComponentIfExists<OtTransformComponent>(newEntity, entity, registry);
	cloneComponentIfExists<OtTransformComponent>(newEntity, entity, registry);
	return newEntity;
}


//
//	OtScene2::getEntity
//

OtEntity OtScene2::getEntity(entt::entity entity) {
	return OtEntity(&registry, entity);
}

OtEntity OtScene2::getEntity(uint64_t id) {
	if (mapIdToEntity.count(id)) {
		return OtEntity(&registry, mapIdToEntity[id]);

	} else {
		return OtEntity();
	}
}

OtEntity OtScene2::getEntity(const std::string &tag) {
	auto view = registry.view<OtTagComponent>();

	for (auto entity : registry.view<OtTagComponent>()) {
		if (view.get<OtTagComponent>(entity).tag == tag) {
			return OtEntity(&registry, entity);
		}
	}

	return OtEntity();
}


//
//	OtScene2::hasEntity
//

bool OtScene2::hasEntity(entt::entity entity) {
	return mapEntityToId.count(entity) != 0;
}

bool OtScene2::hasEntity(uint64_t id) {
	return mapIdToEntity.count(id) != 0;
}

bool OtScene2::hasEntity(const std::string &tag) {
	return getEntity(tag).isValid();
}
