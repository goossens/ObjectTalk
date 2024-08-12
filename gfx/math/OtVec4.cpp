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
#include "OtInteger.h"
#include "OtReal.h"

#include "OtVec4.h"


//
//	OtVec4Class::init
//

void OtVec4Class::init(size_t count, OtObject* parameters) {
	if (count == 4) {
		value = glm::vec4(
			parameters[0]->operator float(),
			parameters[1]->operator float(),
			parameters[2]->operator float(),
			parameters[3]->operator float());

	} else if (count == 1) {
		value = glm::vec4(parameters[0]->operator float());

	} else if (count != 0) {
		OtError("[Vec4] constructor expects 0, 1 or 4 arguments (not {})", count);
	}
}


//
//	OtVec4Class::getMeta
//

OtType OtVec4Class::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtType::create<OtVec4Class>("Vec4", OtObjectClass::getMeta());
		type->set("__init__", OtFunction::create(&OtVec4Class::init));

		type->set("setX", OtFunction::create(&OtVec4Class::setX));
		type->set("setY", OtFunction::create(&OtVec4Class::setY));
		type->set("setZ", OtFunction::create(&OtVec4Class::setZ));
		type->set("setW", OtFunction::create(&OtVec4Class::setW));

		type->set("getX", OtFunction::create(&OtVec4Class::getX));
		type->set("getY", OtFunction::create(&OtVec4Class::getY));
		type->set("getZ", OtFunction::create(&OtVec4Class::getZ));
		type->set("getW", OtFunction::create(&OtVec4Class::getW));

		type->set("__add__", OtFunction::create(&OtVec4Class::add));
		type->set("__sub__", OtFunction::create(&OtVec4Class::subtract));
		type->set("__mul__", OtFunction::create(&OtVec4Class::multiply));
		type->set("__div__", OtFunction::create(&OtVec4Class::divide));

		type->set("length", OtFunction::create(&OtVec4Class::length));
		type->set("normalize", OtFunction::create(&OtVec4Class::normalize));
		type->set("lerp", OtFunction::create(&OtVec4Class::lerp));
	}

	return type;
}
