//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtFunction.h"

#include "OtAnimation.h"
#include "OtAnimator.h"
#include "OtEntityObject.h"
#include "OtSceneObject.h"
#include "OtTransformComponentObject.h"


//
//	OtEntityObjectClass::linkToECS
//

void OtEntityObjectClass::linkToECS(OtScene* s, OtEntity e) {
	scene = s;
	entity = e;
}


//
//	OtEntityObjectClass::entityExists
//

bool OtEntityObjectClass::entityExists(const std::string& tag) {
	return scene->hasEntity(tag);
}


//
//	OtEntityObjectClass::getEntity
//

OtObject OtEntityObjectClass::getEntity(const std::string& tag) {
	// get the entity
	auto entity = scene->getEntity(tag);

	// see if this entity has a script component
	if (scene->hasComponent<OtScriptComponent>(entity)) {
		// we use the script instance
		return scene->getComponent<OtScriptComponent>(entity).instance;

	} else {
		// create a new entity reference
		return OtEntityObject::create(scene, entity);
	}
}


//
//	OtEntityObjectClass::hasTransformComponent
//

bool OtEntityObjectClass::hasTransformComponent() {
	return scene->hasComponent<OtTransformComponent>(entity);
}


//
//	OtEntityObjectClass::getTransformComponent
//

OtObject OtEntityObjectClass::getTransformComponent() {
	return OtTransformComponentObject::create(&scene->getComponent<OtTransformComponent>(entity));
}


//
//	OtEntityObjectClass::getMeta
//

OtType OtEntityObjectClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtType::create<OtEntityObjectClass>("Entity", OtObjectClass::getMeta());

		type->set("entityExists", OtFunction::create(&OtEntityObjectClass::entityExists));
		type->set("getEntity", OtFunction::create(&OtEntityObjectClass::getEntity));

		type->set("hasTransformComponent", OtFunction::create(&OtEntityObjectClass::hasTransformComponent));
		type->set("getTransformComponent", OtFunction::create(&OtEntityObjectClass::getTransformComponent));
	}

	return type;
}
