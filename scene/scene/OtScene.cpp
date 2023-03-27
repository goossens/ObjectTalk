//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <fstream>
#include <sstream>

#include "nlohmann/json.hpp"

#include "OtException.h"

#include "OtScene.h"


//
//	OtScene::load
//

void OtScene::load(const std::filesystem::path& path) {
	// load scene from file
	std::stringstream buffer;

	try {
		std::ifstream stream(path.c_str());

		if (stream.fail()) {
			OtError("Can't read from file [%s]", path.c_str());
		}

		buffer << stream.rdbuf();
		stream.close();

	} catch (std::exception& e) {
		OtError("Can't read from file [%s], error: %s", path.c_str(), e.what());
	}

	// recreate the scene
	auto basedir = path.parent_path();
	deserialize(buffer.str(), &basedir);
}


//
//	OtScene::save
//

void OtScene::save(const std::filesystem::path& path) {
	try {
		// write scene to file
		std::ofstream stream(path.c_str());

		if (stream.fail()) {
			OtError("Can't open file [%s] for writing", path.c_str());
		}

		auto basedir = path.parent_path();
		stream << serialize(1, '\t', &basedir);
		stream.close();

	} catch (std::exception& e) {
		OtError("Can't write to file [%s], error: %s", path.c_str(), e.what());
	}
}


//
//	OtScene::serialize
//

std::string OtScene::serialize(int indent, char character, std::filesystem::path* basedir) {
	// write entities and components
	auto data = nlohmann::json::array();

	eachChild(getRootEntity(), [&](OtEntity entity) {
		data.push_back(serializeEntityToJson(entity, basedir));
	});

	return data.dump(indent, character);
}


//
//	OtScene::deserialize
//

void OtScene::deserialize(const std::string& data, std::filesystem::path* basedir) {
	auto tree = nlohmann::json::parse(data);

	for (auto& entity : tree) {
		addEntityToParent(getRootEntity(), deserializeEntityFromJson(entity, basedir));
	}
}


//
//	OtScene::serializeEntity
//

std::string OtScene::serializeEntity(OtEntity entity, int indent, char character, std::filesystem::path* basedir) {
	return serializeEntityToJson(entity, basedir).dump(indent, character);
}


//
//	OtScene::deserializeEntity
//

OtEntity OtScene::deserializeEntity(const std::string &json, std::filesystem::path* basedir) {
	auto data = nlohmann::json::parse(json);
	return deserializeEntityFromJson(data, basedir);
}


//
//	OtScene::serializeEntityToJson
//

template<typename T>
static inline void serializeComponentToJson(nlohmann::json& json, OtScene* scene, OtEntity entity, std::filesystem::path* basedir) {
	if (scene->hasComponent<T>(entity)) {
		json.push_back(scene->getComponent<T>(entity).serialize(basedir));
	}
}

template<typename... T>
static inline void serializeComponentsToJson(nlohmann::json& json, OtScene* scene, OtEntity entity, std::filesystem::path* basedir) {
	(serializeComponentToJson<T>(json, scene, entity, basedir), ...);
}

nlohmann::json OtScene::serializeEntityToJson(OtEntity entity, std::filesystem::path* basedir) {
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
static inline void deserializeComponentFromJson(nlohmann::json& json, OtScene* scene, OtEntity entity, std::filesystem::path* basedir) {
	if (json["component"] == T::name) {
		scene->getOrAddComponent<T>(entity).deserialize(json, basedir);
	}
}

template<typename... T>
static inline void deserializeComponentsFromJson(nlohmann::json& json, OtScene* scene, OtEntity entity, std::filesystem::path* basedir) {
	(deserializeComponentFromJson<T>(json, scene, entity, basedir), ...);
}

OtEntity OtScene::deserializeEntityFromJson(nlohmann::json &data, std::filesystem::path* basedir) {
	// create a new entity
	auto entity = createEntity();

	// deserialize all its components
	for (auto component : data["components"]) {
		deserializeComponentFromJson<OtCoreComponent>(component, this, entity, basedir);
		deserializeComponentsFromJson<OtSceneComponents>(component, this, entity, basedir);
	}

	// deserialize all its children
	for (auto child : data["children"]) {
		addEntityToParent(entity, deserializeEntityFromJson(child, basedir));
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
