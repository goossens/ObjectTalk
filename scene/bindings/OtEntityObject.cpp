//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtClass.h"
#include "OtFunction.h"

#include "OtEntityObject.h"
#include "OtMessageComponentObject.h"
#include "OtModelComponentObject.h"
#include "OtTransformComponentObject.h"


//
//	OtEntityObjectClass::linkToECS
//

void OtEntityObjectClass::linkToECS(OtScene* s, OtEntity e) {
	scene = s;
	entity = e;
}


//
//	OtEntityObjectClass::hasModelComponent
//

bool OtEntityObjectClass::hasModelComponent() {
	return scene->hasComponent<OtModelComponent>(entity);
}


//
//	OtEntityObjectClass::getModelComponent
//

OtObject OtEntityObjectClass::getModelComponent() {
	return OtModelComponentObject::create(&scene->getComponent<OtModelComponent>(entity));
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
//	OtEntityObjectClass::hasMessageComponent
//

bool OtEntityObjectClass::hasMessageComponent() {
	return scene->hasComponent<OtMessageComponent>(entity);
}


//
//	OtEntityObjectClass::getMessageComponent
//

OtObject OtEntityObjectClass::getMessageComponent() {
	return OtMessageComponentObject::create(&scene->getComponent<OtMessageComponent>(entity));
}


//
//	OtEntityObjectClass::getMeta
//

OtType OtEntityObjectClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtType::create<OtEntityObjectClass>("Entity", OtObjectClass::getMeta());

		type->set("hasModelComponent", OtFunction::create(&OtEntityObjectClass::hasModelComponent));
		type->set("getModelComponent", OtFunction::create(&OtEntityObjectClass::getModelComponent));

		type->set("hasTransformComponent", OtFunction::create(&OtEntityObjectClass::hasTransformComponent));
		type->set("getTransformComponent", OtFunction::create(&OtEntityObjectClass::getTransformComponent));

		type->set("hasMessageComponent", OtFunction::create(&OtEntityObjectClass::hasMessageComponent));
		type->set("getMessageComponent", OtFunction::create(&OtEntityObjectClass::getMessageComponent));
	}

	return type;
}
