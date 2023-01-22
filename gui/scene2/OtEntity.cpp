//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "nlohmann/json.hpp"

#include "OtComponents.h"
#include "OtEntity.h"
#include "OtScene2.h"


//
//	OtEntitySerialize
//

template<typename T>
static inline void SerializeComponent(nlohmann::json& json, OtScene2 scene, OtEntity entity) {
	if (scene->hasComponent<T>(entity)) {
		json.push_back(scene->getComponent<T>(entity).serialize());
	}
}

nlohmann::json OtEntitySerialize(OtScene2 scene, OtEntity entity) {
	// serialize the entity information
	auto data = nlohmann::json::object();

	// serialize the components
	auto components = nlohmann::json::array();
	SerializeComponent<OtUuidComponent>(components, scene, entity);
	SerializeComponent<OtNameComponent>(components, scene, entity);
	SerializeComponent<OtTransformComponent>(components, scene, entity);
	data["components"] = components;

	// serialize the children
	auto children = nlohmann::json::array();
	auto hierarchy = nlohmann::json::array();

	scene->eachChild(entity, [&](OtEntity child) {
		children.push_back(OtEntitySerialize(scene, child));
	});

	data["children"] = children;
	return data;
}


//
//	OtEntityDeserialize
//

OtEntity OtEntityDeserialize(OtScene2 scene, nlohmann::json data, bool preserveUuid) {
	// create a new entity
	auto entity = scene->createEntity();

	// create all its components
	for (auto component : data["components"]) {
		auto type = component["type"];

		if (type == "uuid") {
			if (preserveUuid) {
				scene->updateEntityUuid(entity, component["uuid"]);
			}

		} else if (type == "name") {
			scene->getComponent<OtNameComponent>(entity).deserialize(component);

		} else if (type == "transform") {
			scene->addComponent<OtTransformComponent>(entity).deserialize(component);
		}
	}

	// create all its children
	for (auto child : data["children"]) {
		scene->addEntityToParent(entity, OtEntityDeserialize(scene, child, preserveUuid));
	}

	return entity;
}


//
//	OtEntitySerializeToString
//

std::string OtEntitySerializeToString(OtScene2 scene, OtEntity entity) {
	return OtEntitySerialize(scene, entity).dump();
}


//
//	OtEntityDeserializeFromString
//

OtEntity OtEntityDeserializeFromString(OtScene2 scene, const std::string &data, bool preserveUuid) {
	return OtEntityDeserialize(scene, nlohmann::json::parse(data), preserveUuid);
}
