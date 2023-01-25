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
	SerializeComponent<OtTagComponent>(components, scene, entity);
	SerializeComponent<OtCameraComponent>(components, scene, entity);
	SerializeComponent<OtTransformComponent>(components, scene, entity);
	data["components"] = components;

	// serialize the children
	auto children = nlohmann::json::array();

	scene->eachChild(entity, [&](OtEntity child) {
		children.push_back(OtEntitySerialize(scene, child));
	});

	data["children"] = children;
	return data;
}


//
//	OtEntityDeserialize
//

OtEntity OtEntityDeserialize(OtScene2 scene, nlohmann::json data) {
	// create a new entity
	auto entity = scene->createEntity();

	// create all its components
	for (auto component : data["components"]) {
		auto type = component["component"];

		if (type == OtUuidComponent::name) {
			scene->getComponent<OtUuidComponent>(entity).deserialize(component);

		} else if (type == OtTagComponent::name) {
			scene->getComponent<OtTagComponent>(entity).deserialize(component);

		} else if (type == OtCameraComponent::name) {
			scene->addComponent<OtCameraComponent>(entity).deserialize(component);

		} else if (type == OtTransformComponent::name) {
			scene->addComponent<OtTransformComponent>(entity).deserialize(component);

		} else if (type == OtCameraComponent::name) {
			scene->addComponent<OtCameraComponent>(entity).deserialize(component);
		}
	}

	// create all its children
	for (auto child : data["children"]) {
		scene->addEntityToParent(entity, OtEntityDeserialize(scene, child));
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

OtEntity OtEntityDeserializeFromString(OtScene2 scene, const std::string &data) {
	return OtEntityDeserialize(scene, nlohmann::json::parse(data));
}
