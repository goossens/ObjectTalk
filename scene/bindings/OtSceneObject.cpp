//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtFunction.h"
#include "OtLog.h"

#include "OtEntityObject.h"
#include "OtComponentObject.h"


//
//	OtSceneObjectClass::getEntity
//

OtObject OtSceneObjectClass::getEntity(const std::string& tag) {
	// get the entity
	auto entity = scene->getEntity(tag);

	// see if this entity has a script component
	if (scene->hasComponent<OtScriptComponent>(entity)) {
		// yes it does so we use the script's instance (which is derived from Entity)
		return scene->getComponent<OtScriptComponent>(entity).instance;

	} else {
		// create a new entity object
		return OtEntityObject::create(scene, entity);
	}
}


//
//	OtSceneObjectClass::addEntity
//

OtObject OtSceneObjectClass::addEntity(const std::string& tag) {
	if (hasEntity(tag)) {
		OtLogError("Entity with tag [{}] already exists in scene", tag);
	}

	return OtEntityObject::create(scene, scene->createEntity(0, tag));
}


//
//	OtSceneObjectClass::deleteEntity
//

void OtSceneObjectClass::deleteEntity(OtObject object) {
	scene->removeEntity(toEntity(object));
}


//
//	OtSceneObjectClass::getRootEntity
//

OtObject OtSceneObjectClass::getRootEntity() {
	return OtEntityObject::create(scene, scene->getRootEntity());
}


//
//	OtSceneObjectClass::toEntity
//

OtEntity OtSceneObjectClass::toEntity(OtObject object) {
	if (object->isKindOf("Entity")) {
		auto entity = OtEntityObject(object)->entity;

		if (scene->isValidEntity(entity)) {
			return entity;

		} else {
			OtLogError("Entity not found in scene");
		}

	} else if (object->isKindOf("String")) {
		auto tag = object->operator std::string();

		if (scene->hasEntity(tag)) {
			return scene->getEntity(tag);

		} else {
			OtLogError("No entity found in scene with tag [{}]", tag);
		}

	} else {
		OtLogError("Object of class [{}] can't be used to reference a scene entity", object->getTypeName());
	}

	return OtEntityNull;
}


//
//	OtSceneObjectClass::getMeta
//

OtType OtSceneObjectClass::getMeta()
{
	static OtType type = nullptr;

	if (!type) {
		type = OtType::create<OtSceneObjectClass>("Scene", OtObjectClass::getMeta());
		type->set("load", OtFunction::create(&OtSceneObjectClass::load));
		type->set("save", OtFunction::create(&OtSceneObjectClass::save));

		type->set("hasEntity", OtFunction::create(&OtSceneObjectClass::hasEntity));
		type->set("getEntity", OtFunction::create(&OtSceneObjectClass::getEntity));
		type->set("addEntity", OtFunction::create(&OtSceneObjectClass::addEntity));
		type->set("deleteEntity", OtFunction::create(&OtSceneObjectClass::deleteEntity));

		type->set("getRootEntity", OtFunction::create(&OtSceneObjectClass::getRootEntity));
	}

	return type;
}
