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

#include "OtVec2.h"


//
//	OtVec2Class::init
//

void OtVec2Class::init(size_t count, OtObject* parameters) {
	if (count == 2) {
		vec2 = glm::vec2(parameters[0]->operator float(), parameters[1]->operator float());

	} else if (count == 1) {
		vec2 = glm::vec2(parameters[0]->operator float());

	} else if (count != 0) {
		OtError("[Vec2] constructor expects 0, 1 or 2 arguments (not %ld)", count);
	}
}


//
//	OtVec2Class::add
//

OtObject OtVec2Class::add(OtObject object) {
	if (object.isKindOf<OtVec2Class>()) {
		OtVec2 vector2 = OtVec2(object);
		return OtVec2::create(vec2 + vector2->vec2);

	} else if (object.isKindOf<OtIntegerClass>() || object.isKindOf<OtRealClass>()) {
		return OtVec2::create(vec2 + object->operator float());

	} else {
		OtError("Expected a [Vec2], [Real] or [Integer] as the operand, not a [%s]", object->getType()->getName().c_str());
		return nullptr;
	}
}


//
//	OtVec2Class::subtract
//

OtObject OtVec2Class::subtract(OtObject object) {
	if (object.isKindOf<OtVec2Class>()) {
		OtVec2 vector2 = OtVec2(object);
		return OtVec2::create(vec2 - vector2->vec2);

	} else if (object.isKindOf<OtIntegerClass>() || object.isKindOf<OtRealClass>()) {
		return OtVec2::create(vec2 - object->operator float());

	} else {
		OtError("Expected a [Vec2], [Real] or [Integer] as the operand, not a [%s]", object->getType()->getName().c_str());
		return nullptr;
	}
}


//
//	OtVec2Class::multiply
//

OtObject OtVec2Class::multiply(OtObject object) {
	if (object.isKindOf<OtVec2Class>()) {
		OtVec2 vector2 = OtVec2(object);
		return OtVec2::create(vec2 * vector2->vec2);

	} else if (object.isKindOf<OtIntegerClass>() || object.isKindOf<OtRealClass>()) {
		return OtVec2::create(vec2 * object->operator float());

	} else {
		OtError("Expected a [Vec2], [Real] or [Integer] as the operand, not a [%s]", object->getType()->getName().c_str());
		return nullptr;
	}
}


//
//	OtVec2Class::divide
//

OtObject OtVec2Class::divide(OtObject object) {
	if (object.isKindOf<OtVec2Class>()) {
		OtVec2 vector2 = OtVec2(object);
		return OtVec2::create(vec2 / vector2->vec2);

	} else if (object.isKindOf<OtIntegerClass>() || object.isKindOf<OtRealClass>()) {
		return OtVec2::create(vec2 / object->operator float());

	} else {
		OtError("Expected a [Vec2], [Real] or [Integer] as the operand, not a [%s]", object->getType()->getName().c_str());
		return nullptr;
	}
}


//
//	OtVec2Class::length
//

float OtVec2Class::length() {
	return glm::length(vec2);
}


//
//	OtVec2Class::normalize
//

OtObject OtVec2Class::normalize() {
	return OtVec2::create(glm::normalize(vec2));
}


//
//	OtVec2Class::distance
//

float OtVec2Class::distance(OtObject object) {
	if (object.isKindOf<OtVec2Class>()) {
		OtVec2 vector2 = OtVec2(object);
		return glm::distance(vec2, vector2->vec2);

	} else {
		OtError("Expected a [Vec2] as the operand, not a [%s]", object->getType()->getName().c_str());
		return 0.0f;
	}

}


//
//	OtVec2Class::lerp
//

OtObject OtVec2Class::lerp(OtObject object, float delta) {
	if (object.isKindOf<OtVec2Class>()) {
		if (delta < 0.0f) {
			return OtVec2(this);

		} else if (delta > 1.0f) {
			return object;

		} else {
			OtVec2 vector2 = OtVec2(object);
			return OtVec2::create(vec2 + ((vector2->vec2 - vec2) * delta));
		}

	} else {
		OtError("Expected a [Vec2] as the operand, not a [%s]", object->getType()->getName().c_str());
		return nullptr;
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

		type->set("length", OtFunction::create(&OtVec2Class::length));
		type->set("normalize", OtFunction::create(&OtVec2Class::normalize));
		type->set("distance", OtFunction::create(&OtVec2Class::distance));
		type->set("lerp", OtFunction::create(&OtVec2Class::lerp));
	}

	return type;
}
