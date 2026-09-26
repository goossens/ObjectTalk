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
#include "OtSceneObject.h"


//
//	OtSceneObjectClass::hasEntity
//

bool OtSceneObjectClass::hasEntity(const std::string& tag) {
	return scene->hasEntity(tag);
}


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
	object.expect<OtEntityObjectClass>("Entity");
	scene->removeEntity(OtEntityObject(object)->entity);

}


//
//	OtSceneObjectClass::getRootEntity
//

OtObject OtSceneObjectClass::getRootEntity() {
	return OtEntityObject::create(scene, scene->getRootEntity());
}


//
//	OtSceneObjectClass::getMeta
//

OtType OtSceneObjectClass::getMeta()
{
	static OtType type = nullptr;

	if (!type) {
		type = OtType::create<OtSceneObjectClass>("Scene", OtObjectClass::getMeta());

		type->set("hasEntity", OtFunction::create(&OtSceneObjectClass::hasEntity));
		type->set("getEntity", OtFunction::create(&OtSceneObjectClass::getEntity));
		type->set("addEntity", OtFunction::create(&OtSceneObjectClass::addEntity));
		type->set("deleteEntity", OtFunction::create(&OtSceneObjectClass::deleteEntity));

		type->set("getRootEntity", OtFunction::create(&OtSceneObjectClass::getRootEntity));
	}

	return type;
}
