//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtException.h"
#include "OtFunction.h"

#include "OtVec3.h"

#include "OtTransformComponentObject.h"


//
//	OtTransformComponentObjectClass::setTranslation
//

void OtTransformComponentObjectClass::setTranslation(OtObject object) {
	// sanity check
	if (!object.isKindOf<OtVec3Class>()) {
		OtError("Expected a [Vec3], not a [{}]", object->getType()->getName());
	}

	// set new value
	transform->translation = *OtVec3(object);
}


//
//	OtTransformComponentObjectClass::setRotation
//

void OtTransformComponentObjectClass::setRotation(OtObject object) {
	// sanity check
	if (!object.isKindOf<OtVec3Class>()) {
		OtError("Expected a [Vec3], not a [{}]", object->getType()->getName());
	}

	// set new value
	transform->rotation = *OtVec3(object);
}


//
//	OtTransformComponentObjectClass::setScale
//

void OtTransformComponentObjectClass::setScale(OtObject object) {
	// sanity check
	if (!object.isKindOf<OtVec3Class>()) {
		OtError("Expected a [Vec3], not a [{}]", object->getType()->getName());
	}

	// set new value
	transform->scale = *OtVec3(object);
}


//
//	OtTransformComponentObjectClass::getTranslation
//

OtObject OtTransformComponentObjectClass::getTranslation() {
	return OtVec3::create(transform->translation);
}


//
//	OtTransformComponentObjectClass::getRotation
//

OtObject OtTransformComponentObjectClass::getRotation() {
	return OtVec3::create(transform->rotation);
}


//
//	OtTransformComponentObjectClass::getScale
//

OtObject OtTransformComponentObjectClass::getScale() {
	return OtVec3::create(transform->scale);

}


//
//	OtTransformComponentObjectClass::getMeta
//

OtType OtTransformComponentObjectClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtType::create<OtTransformComponentObjectClass>("TransformComponent", OtObjectClass::getMeta());

		type->set("setTranslation", OtFunction::create(&OtTransformComponentObjectClass::setTranslation));
		type->set("setRotation", OtFunction::create(&OtTransformComponentObjectClass::setRotation));
		type->set("setScale", OtFunction::create(&OtTransformComponentObjectClass::setScale));

		type->set("getTranslation", OtFunction::create(&OtTransformComponentObjectClass::getTranslation));
		type->set("getRotation", OtFunction::create(&OtTransformComponentObjectClass::getRotation));
		type->set("getScale", OtFunction::create(&OtTransformComponentObjectClass::getScale));
	}

	return type;
}
