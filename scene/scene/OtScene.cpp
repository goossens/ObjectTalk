//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "nlohmann/json.hpp"

#include "OtLog.h"
#include "OtPath.h"
#include "OtText.h"

#include "OtScene.h"


//
//	OtScene::load
//

void OtScene::load(const std::string& path) {
	// load scene from file
	std::string text;
	OtText::load(path, text);

	// clear scene
	clear();

	// treat an empty file as a blank scene
	if (text.size()) {
		// parse json
		auto data = nlohmann::json::parse(text);

		// extract metadata
		if (data.contains("metadata")) {
			metadata = data["metadata"].dump();
		}

		// extract entities
		auto basedir = OtPath::getParent(path);

		for (auto& entity : data["entities"]) {
			addEntityToParent(getRootEntity(), deserializeEntityFromJson(entity, &basedir, true));
		}

		// extract post processing settings
		if (data.contains("processing")) {
			postProcessing.deserialize(data["processing"], &basedir);
		}
	}
}


//
//	OtScene::save
//

void OtScene::save(const std::string& path) {
	// create json data structure and add metadata
	auto data = nlohmann::json::object();
	data["metadata"] = nlohmann::json::parse(metadata.c_str());

	// write entities and components
	auto entities = nlohmann::json::array();
	auto basedir = OtPath::getParent(path);

	eachChild(getRootEntity(), [&](OtEntity entity) {
		entities.push_back(serializeEntityToJson(entity, &basedir));
	});

	data["entities"] = entities;

	// write post processing settings
	data["processing"] = postProcessing.serialize(&basedir);

	// write scene to file
	OtText::save(path, data.dump(1, '\t'));
}


//
//	OtScene::clear
//

void OtScene::clear() {
	OtEcs::clear();
	metadata = "{}";
}


//
//	OtScene::archiveEntity
//

std::string OtScene::archiveEntity(OtEntity entity, int indent, char character, std::string* basedir) {
	return serializeEntityToJson(entity, basedir).dump(indent, character);
}


//
//	OtScene::restoreEntity
//

OtEntity OtScene::restoreEntity(const std::string& json, std::string* basedir) {
	auto data = nlohmann::json::parse(json);
	return deserializeEntityFromJson(data, basedir, true);
}


//
//	OtScene::duplicateEntity
//

OtEntity OtScene::duplicateEntity(const std::string& json, std::string* basedir) {
	auto data = nlohmann::json::parse(json);
	return deserializeEntityFromJson(data, basedir, false);
}


//
//	OtScene::serializeEntityToJson
//

template<typename T>
static inline void serializeComponentToJson(nlohmann::json& json, OtScene* scene, OtEntity entity, std::string* basedir) {
	if (scene->hasComponent<T>(entity)) {
		json.push_back(scene->getComponent<T>(entity).serialize(basedir));
	}
}

template<typename... T>
static inline void serializeComponentsToJson(nlohmann::json& json, OtScene* scene, OtEntity entity, std::string* basedir) {
	(serializeComponentToJson<T>(json, scene, entity, basedir), ...);
}

nlohmann::json OtScene::serializeEntityToJson(OtEntity entity, std::string* basedir) {
	// serialize the entity's components
	auto data = nlohmann::json::object();
	auto components = nlohmann::json::array();
	serializeComponentToJson<OtCoreComponent>(components, this, entity, basedir);
	serializeComponentsToJson<OtSceneComponents>(components, this, entity, basedir);
	data["components"] = components;

	// serialize the entity's children
	auto children = nlohmann::json::array();

	eachChild(entity, [&](OtEntity child) {
		children.push_back(serializeEntityToJson(child, basedir));
	});

	data["children"] = children;
	return data;
}


//
//	OtScene::deserializeEntityFromJson
//

template<typename T>
static inline void deserializeComponentFromJson(nlohmann::json& json, OtScene* scene, OtEntity entity, std::string* basedir) {
	if (json["component"] == T::name) {
		scene->getOrAddComponent<T>(entity).deserialize(json, basedir);
	}
}

template<typename... T>
static inline void deserializeComponentsFromJson(nlohmann::json& json, OtScene* scene, OtEntity entity, std::string* basedir) {
	(deserializeComponentFromJson<T>(json, scene, entity, basedir), ...);
}

OtEntity OtScene::deserializeEntityFromJson(nlohmann::json& data, std::string* basedir, bool preserveUuid) {
	// create a new entity
	auto entity = createEntity();
	auto tmpUuid = getEntityUuid(entity);

	// deserialize all its components
	for (auto component : data["components"]) {
		deserializeComponentFromJson<OtCoreComponent>(component, this, entity, basedir);
		deserializeComponentsFromJson<OtSceneComponents>(component, this, entity, basedir);
	}

	// do we need to preserve the UUID?
	if (preserveUuid) {
		// remap the entity's UUID ("stored" UUID is different from "created" UUID)
		remapEntityUuid(entity, tmpUuid, getEntityUuid(entity));

	} else {
		// replace the "restored" UUID with the "created" UUID
		auto& component = getComponent<OtCoreComponent>(entity);
		component.uuid = tmpUuid;
	}

	// deserialize all its children
	for (auto child : data["children"]) {
		addEntityToParent(entity, deserializeEntityFromJson(child, basedir, preserveUuid));
	}

	return entity;
}


//
//	OtScene::getGlobalTransform
//

glm::mat4 OtScene::getGlobalTransform(OtEntity entity) {
	if (hasComponent<OtTransformComponent>(entity)) {
		auto& transform = getComponent<OtTransformComponent>(entity);
		auto parent = getParent(entity);

		if (isValidEntity(parent)) {
			return getGlobalTransform(parent) * transform.getTransform();

		} else {
			return transform.getTransform();
		}

	} else {
		return glm::mat4(1.0f);
	}
}


//
//	OtScene::evaluateNodes
//

void OtScene::evaluateNodes() {
	// evaluate the nodes
	for (auto&& [entity, component] : view<OtNodesComponent>().each()) {
		component.nodes.evaluate();
	}
}
