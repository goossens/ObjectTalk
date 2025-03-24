//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtFunction.h"

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
//	OtSceneObjectClass::getMeta
//

OtType OtSceneObjectClass::getMeta()
{
	static OtType type = nullptr;

	if (!type) {
		type = OtType::create<OtSceneObjectClass>("Scene", OtObjectClass::getMeta());

		type->set("hasEntity", OtFunction::create(&OtSceneObjectClass::hasEntity));
		type->set("getEntity", OtFunction::create(&OtSceneObjectClass::getEntity));
	}

	return type;
}
