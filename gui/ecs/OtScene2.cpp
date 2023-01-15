//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
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
//	createID
//

static uint64_t createID() {
	static std::random_device device;
	static std::mt19937_64 engine(device());
	static std::uniform_int_distribution<uint64_t> distribution;
	return distribution(engine);
}


//
//	OtScene2Class::OtScene2Class
//

OtScene2Class::OtScene2Class() {
	id = createID();
}


//
//	OtScene2::~OtScene2
//

OtScene2Class::~OtScene2Class() {
}


//
//	OtScene2Class::createEntity
//

OtEntity OtScene2Class::createEntity(const std::string& name) {
	auto entity = OtEntity(&registry, registry.create());
	auto entityId = createID();
	entity.addComponent<OtIdComponent>(entityId);
	entity.addComponent<OtNameComponent>(name);
	entity.addComponent<OtTransformComponent>();
	mapIdToEntity[entityId] = entity;
	mapEntityToId[entity] = entityId;
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
	OtEntity newEntity;

	if (entity.hasComponent<OtNameComponent>()) {
		newEntity = createEntity(entity.getComponent<OtNameComponent>().name);

	} else {
		newEntity = createEntity();
	}

	cloneComponentIfExists<OtNameComponent>(newEntity, entity, registry);
	cloneComponentIfExists<OtTransformComponent>(newEntity, entity, registry);
	return newEntity;
}


//
//	OtScene2::getEntity
//

OtEntity OtScene2Class::getEntity(entt::entity entity) {
	return OtEntity(&registry, entity);
}

OtEntity OtScene2Class::getEntity(uint64_t id) {
	if (mapIdToEntity.count(id)) {
		return OtEntity(&registry, mapIdToEntity[id]);

	} else {
		return OtEntity();
	}
}

OtEntity OtScene2Class::getEntity(const std::string &name) {
	auto view = registry.view<OtNameComponent>();

	for (auto entity : registry.view<OtNameComponent>()) {
		if (view.get<OtNameComponent>(entity).name == name) {
			return OtEntity(&registry, entity);
		}
	}

	return OtEntity();
}


//
//	OtScene2Class::hasEntity
//

bool OtScene2Class::hasEntity(entt::entity entity) {
	return mapEntityToId.count(entity) != 0;
}

bool OtScene2Class::hasEntity(uint64_t id) {
	return mapIdToEntity.count(id) != 0;
}

bool OtScene2Class::hasEntity(const std::string &name) {
	return getEntity(name).isValid();
}


//
//	OtScene2Class::removeEntity
//

void OtScene2Class::removeEntity(OtEntity entity) {
	registry.destroy(entity);
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
