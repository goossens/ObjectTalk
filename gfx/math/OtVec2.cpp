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

#include "OtVec2.h"


//
//	OtVec2Class::init
//

void OtVec2Class::init(size_t count, OtObject* parameters) {
	if (count == 2) {
		value = glm::vec2(parameters[0]->operator float(), parameters[1]->operator float());

	} else if (count == 1) {
		value = glm::vec2(parameters[0]->operator float());

	} else if (count != 0) {
		OtError("[Vec2] constructor expects 0, 1 or 2 arguments (not {})", count);
	}
}


//
//	OtVec2Class::getMeta
//

OtType OtVec2Class::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtType::create<OtVec2Class>("Vec2", OtObjectClass::getMeta());
		type->set("__init__", OtFunction::create(&OtVec2Class::init));

		type->set("setX", OtFunction::create(&OtVec2Class::setX));
		type->set("setY", OtFunction::create(&OtVec2Class::setY));

		type->set("getX", OtFunction::create(&OtVec2Class::getX));
		type->set("getY", OtFunction::create(&OtVec2Class::getY));

		type->set("__add__", OtFunction::create(&OtVec2Class::add));
		type->set("__sub__", OtFunction::create(&OtVec2Class::subtract));
		type->set("__mul__", OtFunction::create(&OtVec2Class::multiply));
		type->set("__div__", OtFunction::create(&OtVec2Class::divide));

		type->set("__eq__", OtFunction::create(&OtVec2Class::equal));
		type->set("__ne__", OtFunction::create(&OtVec2Class::notEqual));

		type->set("length", OtFunction::create(&OtVec2Class::length));
		type->set("normalize", OtFunction::create(&OtVec2Class::normalize));
		type->set("distance", OtFunction::create(&OtVec2Class::distance));
		type->set("lerp", OtFunction::create(&OtVec2Class::lerp));
	}

	return type;
}
