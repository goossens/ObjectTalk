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
#include "OtCameraComponentObject.h"
#include "OtMaterialComponentObject.h"
#include "OtMessageComponentObject.h"
#include "OtModelComponentObject.h"
#include "OtScriptComponentObject.h"
#include "OtTransformComponentObject.h"
#include "OtSceneObject.h"


//
//	OtEntityObjectClass::getScene
//

OtObject OtEntityObjectClass::getScene() {
	return OtSceneObject::create(scene);
}


//
//	Implement component methods

#define implementComponent(t)									\
bool OtEntityObjectClass::has ## t ## Component() { 			\
	return scene->hasComponent<Ot ## t ## Component>(entity);	\
}																\
																\
OtObject OtEntityObjectClass::get ## t ## Component() {			\
	return Ot ## t ## ComponentObject::create(scene, entity);	\
}																\
																\
OtObject OtEntityObjectClass::add ## t ## Component() {			\
	if (has ## t ## Component()) {								\
		OtLogError("Entity already has a " #t "Component]");	\
	}															\
																\
	scene->addComponent<Ot ## t ## Component>(entity);			\
	return Ot ## t ## ComponentObject::create(scene, entity);	\
}

implementComponent(Camera)
implementComponent(Material)
implementComponent(Message)
implementComponent(Model)
implementComponent(Script)
implementComponent(Transform)


//
//	OtEntityObjectClass::toEntity
//

OtEntity OtEntityObjectClass::toEntity(OtObject object) {
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
//	OtEntityObjectClass::toObject
//

OtObject OtEntityObjectClass::toObject(OtEntity targetEntity) {
	return OtEntityObject::create(scene, targetEntity);
}


//
//	OtEntityObjectClass::getMeta
//

#define metaComponent(t)																					\
		type->set("has" #t "Component", OtFunction::create(&OtEntityObjectClass::has## t ## Component));	\
		type->set("get" #t "Component", OtFunction::create(&OtEntityObjectClass::get## t ## Component));	\
		type->set("add" #t "Component", OtFunction::create(&OtEntityObjectClass::add## t ## Component));

OtType OtEntityObjectClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtType::create<OtEntityObjectClass>("Entity", OtObjectClass::getMeta());
		type->set("getScene", OtFunction::create(&OtEntityObjectClass::getScene));

		type->set("moveBefore", OtFunction::create(&OtEntityObjectClass::moveBefore));
		type->set("moveTo", OtFunction::create(&OtEntityObjectClass::moveTo));

		type->set("hasChildren", OtFunction::create(&OtEntityObjectClass::hasChildren));
		type->set("getTag", OtFunction::create(&OtEntityObjectClass::getTag));

		type->set("getParent", OtFunction::create(&OtEntityObjectClass::getParent));
		type->set("getFirstChild", OtFunction::create(&OtEntityObjectClass::getFirstChild));
		type->set("getLastChild", OtFunction::create(&OtEntityObjectClass::getLastChild));
		type->set("getPreviousSibling", OtFunction::create(&OtEntityObjectClass::getPreviousSibling));
		type->set("getNextSibling", OtFunction::create(&OtEntityObjectClass::getNextSibling));

		metaComponent(Camera)
		metaComponent(Material)
		metaComponent(Message)
		metaComponent(Model)
		metaComponent(Script)
		metaComponent(Transform)
	}

	return type;
}
