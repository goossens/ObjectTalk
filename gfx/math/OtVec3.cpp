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


//
//	OtVec3Class::init
//

void OtVec3Class::init(size_t count, OtObject* parameters) {
	if (count == 3) {
		value = glm::vec3(
			parameters[0]->operator float(),
			parameters[1]->operator float(),
			parameters[2]->operator float());

	} else if (count == 1) {
		value = glm::vec3(parameters[0]->operator float());

	} else if (count != 0) {
		OtError("[Vec3] constructor expects 0, 1 or 3 arguments (not {})", count);
	}
}


//
//	OtVec3Class::getMeta
//

OtType OtVec3Class::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtType::create<OtVec3Class>("Vec3", OtObjectClass::getMeta());
		type->set("__init__", OtFunction::create(&OtVec3Class::init));

		type->set("setX", OtFunction::create(&OtVec3Class::setX));
		type->set("setY", OtFunction::create(&OtVec3Class::setY));
		type->set("setZ", OtFunction::create(&OtVec3Class::setZ));

		type->set("getX", OtFunction::create(&OtVec3Class::getX));
		type->set("getY", OtFunction::create(&OtVec3Class::getY));
		type->set("getZ", OtFunction::create(&OtVec3Class::getZ));

		type->set("__add__", OtFunction::create(&OtVec3Class::add));
		type->set("__sub__", OtFunction::create(&OtVec3Class::subtract));
		type->set("__mul__", OtFunction::create(&OtVec3Class::multiply));
		type->set("__div__", OtFunction::create(&OtVec3Class::divide));

		type->set("__eq__", OtFunction::create(&OtVec3Class::equal));
		type->set("__ne__", OtFunction::create(&OtVec3Class::notEqual));

		type->set("length", OtFunction::create(&OtVec3Class::length));
		type->set("normalize", OtFunction::create(&OtVec3Class::normalize));
		type->set("distance", OtFunction::create(&OtVec3Class::distance));
		type->set("dot", OtFunction::create(&OtVec3Class::dot));
		type->set("cross", OtFunction::create(&OtVec3Class::cross));
		type->set("lerp", OtFunction::create(&OtVec3Class::lerp));
	}

	return type;
}
