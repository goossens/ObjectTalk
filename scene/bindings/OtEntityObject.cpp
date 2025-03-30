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
#include "OtTransformComponentObject.h"


//
//	OtEntityObjectClass::linkToECS
//

void OtEntityObjectClass::linkToECS(OtScene* s, OtEntity e) {
	scene = s;
	entity = e;
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

		type->set("hasTransformComponent", OtFunction::create(&OtEntityObjectClass::hasTransformComponent));
		type->set("getTransformComponent", OtFunction::create(&OtEntityObjectClass::getTransformComponent));
	}

	return type;
}
