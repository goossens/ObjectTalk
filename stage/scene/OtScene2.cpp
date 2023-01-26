//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "nlohmann/json.hpp"

#include "OtScene2.h"


//
//	OtScene2Class::serialize
//

std::string OtScene2Class::serialize(int indent, char character) {
	// write entities and components
	auto data = nlohmann::json::array();

	eachChild(getRootEntity(), [&](OtEntity entity) {
		data.push_back(serializeEntity(entity));
	});

	return data.dump(indent, character);
}


//
//	OtScene2Class::deserialize
//

void OtScene2Class::deserialize(const std::string& data) {
	auto tree = nlohmann::json::parse(data);

	for (auto& entity : tree) {
		addEntityToParent(getRootEntity(), deserializeEntityFromJson(entity));
	}
}


//
//	OtScene2Class::serializeEntity
//

std::string OtScene2Class::serializeEntity(OtEntity entity, int indent, char character) {
	return serializeEntityToJson(entity).dump(indent, character);
}


//
//	OtScene2Class::deserializeEntity
//

OtEntity OtScene2Class::deserializeEntity(const std::string &json) {
	auto data = nlohmann::json::parse(json);
	return deserializeEntityFromJson(data);
}


//
//	OtScene2Class::serializeEntityToJson
//

template<typename T>
static inline void serializeComponentToJson(nlohmann::json& json, OtScene2Class* scene, OtEntity entity) {
	if (scene->hasComponent<T>(entity)) {
		json.push_back(scene->getComponent<T>(entity).serialize());
	}
}

template<typename... T>
static inline void serializeComponentsToJson(nlohmann::json& json, OtScene2Class* scene, OtEntity entity) {
	(serializeComponentToJson<T>(json, scene, entity), ...);
}

nlohmann::json OtScene2Class::serializeEntityToJson(OtEntity entity) {
	// serialize the entity's components
	auto data = nlohmann::json::object();
	auto components = nlohmann::json::array();
	serializeComponentsToJson<OtSceneSaveableComponents>(components, this, entity);
	data["components"] = components;

	// serialize the entity's children
	auto children = nlohmann::json::array();

	eachChild(entity, [&](OtEntity child) {
		children.push_back(serializeEntityToJson(child));
	});

	data["children"] = children;
	return data;
}


//
//	OtScene2Class::deserializeEntityFromJson
//

template<typename T>
static inline void deserializeComponentFromJson(nlohmann::json& json, OtScene2Class* scene, OtEntity entity) {
	if (json["component"] == T::name) {
		scene->getOrAddComponent<T>(entity).deserialize(json);
	}
}

template<typename... T>
static inline void deserializeComponentsFromJson(nlohmann::json& json, OtScene2Class* scene, OtEntity entity) {
	(deserializeComponentFromJson<T>(json, scene, entity), ...);
}

OtEntity OtScene2Class::deserializeEntityFromJson(nlohmann::json &data) {
	// create a new entity
	auto entity = createEntity();

	// deserialize all its components
	for (auto component : data["components"]) {
		deserializeComponentsFromJson<OtSceneSaveableComponents>(component, this, entity);
	}

	// deserialize all its children
	for (auto child : data["children"]) {
		addEntityToParent(entity, deserializeEntityFromJson(child));
	}

	return entity;
}


//
//	OtScene2Class::getMeta
//

OtType OtScene2Class::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtScene2Class>("Scene2", OtWorldClass::getMeta());
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
