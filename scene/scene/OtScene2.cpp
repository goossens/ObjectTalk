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

#include "OtScene2.h"


//
//	OtScene2::load
//

void OtScene2::load(const std::filesystem::path& path) {
	// load scene from file
	std::stringstream buffer;

	try {
		std::ifstream stream(path.c_str());

		if (stream.fail()) {
			OtExcept("Can't read from file [%s]", path.c_str());
		}

		buffer << stream.rdbuf();
		stream.close();

	} catch (std::exception& e) {
		OtExcept("Can't read from file [%s], error: %s", path.c_str(), e.what());
	}

	// recreate the scene
	auto basedir = path.parent_path();
	deserialize(buffer.str(), &basedir);
}


//
//	OtScene2::save
//

void OtScene2::save(const std::filesystem::path& path) {
	try {
		// write scene to file
		std::ofstream stream(path.c_str());

		if (stream.fail()) {
			OtExcept("Can't open file [%s] for writing", path.c_str());
		}

		auto basedir = path.parent_path();
		stream << serialize(1, '\t', &basedir);
		stream.close();

	} catch (std::exception& e) {
		OtExcept("Can't write to file [%s], error: %s", path.c_str(), e.what());
	}
}


//
//	OtScene2::serialize
//

std::string OtScene2::serialize(int indent, char character, std::filesystem::path* basedir) {
	// write entities and components
	auto data = nlohmann::json::array();

	eachChild(getRootEntity(), [&](OtEntity entity) {
		data.push_back(serializeEntityToJson(entity, basedir));
	});

	return data.dump(indent, character);
}


//
//	OtScene2::deserialize
//

void OtScene2::deserialize(const std::string& data, std::filesystem::path* basedir) {
	auto tree = nlohmann::json::parse(data);

	for (auto& entity : tree) {
		addEntityToParent(getRootEntity(), deserializeEntityFromJson(entity, basedir));
	}
}


//
//	OtScene2::serializeEntity
//

std::string OtScene2::serializeEntity(OtEntity entity, int indent, char character, std::filesystem::path* basedir) {
	return serializeEntityToJson(entity, basedir).dump(indent, character);
}


//
//	OtScene2::deserializeEntity
//

OtEntity OtScene2::deserializeEntity(const std::string &json, std::filesystem::path* basedir) {
	auto data = nlohmann::json::parse(json);
	return deserializeEntityFromJson(data, basedir);
}


//
//	OtScene2::serializeEntityToJson
//

template<typename T>
static inline void serializeComponentToJson(nlohmann::json& json, OtScene2* scene, OtEntity entity, std::filesystem::path* basedir) {
	if (scene->hasComponent<T>(entity)) {
		json.push_back(scene->getComponent<T>(entity).serialize(basedir));
	}
}

template<typename... T>
static inline void serializeComponentsToJson(nlohmann::json& json, OtScene2* scene, OtEntity entity, std::filesystem::path* basedir) {
	(serializeComponentToJson<T>(json, scene, entity, basedir), ...);
}

nlohmann::json OtScene2::serializeEntityToJson(OtEntity entity, std::filesystem::path* basedir) {
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
//	OtScene2::deserializeEntityFromJson
//

template<typename T>
static inline void deserializeComponentFromJson(nlohmann::json& json, OtScene2* scene, OtEntity entity, std::filesystem::path* basedir) {
	if (json["component"] == T::name) {
		scene->getOrAddComponent<T>(entity).deserialize(json, basedir);
	}
}

template<typename... T>
static inline void deserializeComponentsFromJson(nlohmann::json& json, OtScene2* scene, OtEntity entity, std::filesystem::path* basedir) {
	(deserializeComponentFromJson<T>(json, scene, entity, basedir), ...);
}

OtEntity OtScene2::deserializeEntityFromJson(nlohmann::json &data, std::filesystem::path* basedir) {
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
//	OtScene2::getGlobalTransform
//

glm::mat4 OtScene2::getGlobalTransform(OtEntity entity) {
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
