//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtFunction.h"

#include "OtVec3.h"

#include "OtTransformComponentObject.h"


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
