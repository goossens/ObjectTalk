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

template<typename... T>
static inline void SerializeComponents(nlohmann::json& json, OtScene2 scene, OtEntity entity) {
	(SerializeComponent<T>(json, scene, entity), ...);
}

nlohmann::json OtEntitySerialize(OtScene2 scene, OtEntity entity) {
	// serialize the entity information
	auto data = nlohmann::json::object();

	// serialize the components
	auto components = nlohmann::json::array();
	SerializeComponents<OtSceneSaveableComponents>(components, scene, entity);
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

template<typename T>
static inline void DeserializeComponent(nlohmann::json& json, OtScene2 scene, OtEntity entity) {
	if (json["component"] == T::name) {
		scene->getOrAddComponent<T>(entity).deserialize(json);
	}
}

template<typename... T>
static inline void DeserializeComponents(nlohmann::json& json, OtScene2 scene, OtEntity entity) {
	(DeserializeComponent<T>(json, scene, entity), ...);
}

OtEntity OtEntityDeserialize(OtScene2 scene, nlohmann::json data) {
	// create a new entity
	auto entity = scene->createEntity();

	// create all its components
	for (auto component : data["components"]) {
		DeserializeComponents<OtSceneSaveableComponents>(component, scene, entity);
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
