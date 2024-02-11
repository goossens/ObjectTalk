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
		vec4 = glm::vec4(
			parameters[0]->operator float(),
			parameters[1]->operator float(),
			parameters[2]->operator float(),
			parameters[3]->operator float());

	} else if (count == 1) {
		vec4 = glm::vec4(parameters[0]->operator float());

	} else if (count != 0) {
		OtError("[Vec4] constructor expects 0, 1 or 4 arguments (not {})", count);
	}
}


//
//	OtVec4Class::add
//

OtObject OtVec4Class::add(OtObject object) {
	if (object.isKindOf<OtVec4Class>()) {
		OtVec4 vector4 = OtVec4(object);
		return OtVec4::create(vec4 + vector4->vec4);

	} else if (object.isKindOf<OtIntegerClass>() || object.isKindOf<OtRealClass>()) {
		return OtVec4::create(vec4 + object->operator float());

	} else {
		OtError("Expected a [Vec4], [Real] or [Integer] as the operand, not a [{}]", object->getType()->getName());
		return nullptr;
	}
}


//
//	OtVec4Class::subtract
//

OtObject OtVec4Class::subtract(OtObject object) {
	if (object.isKindOf<OtVec4Class>()) {
		OtVec4 vector4 = OtVec4(object);
		return OtVec4::create(vec4 - vector4->vec4);

	} else if (object.isKindOf<OtIntegerClass>() || object.isKindOf<OtRealClass>()) {
		return OtVec4::create(vec4 - object->operator float());

	} else {
		OtError("Expected a [Vec4], [Real] or [Integer] as the operand, not a [{}]", object->getType()->getName());
		return nullptr;
	}
}


//
//	OtVec4Class::multiply
//

OtObject OtVec4Class::multiply(OtObject object) {
	if (object.isKindOf<OtVec4Class>()) {
		OtVec4 vector4 = OtVec4(object);
		return OtVec4::create(vec4 * vector4->vec4);

	} else if (object.isKindOf<OtIntegerClass>() || object.isKindOf<OtRealClass>()) {
		return OtVec4::create(vec4 * object->operator float());

	} else {
		OtError("Expected a [Vec4], [Real] or [Integer] as the operand, not a [{}]", object->getType()->getName());
		return nullptr;
	}
}


//
//	OtVec4Class::divide
//

OtObject OtVec4Class::divide(OtObject object) {
	if (object.isKindOf<OtVec4Class>()) {
		OtVec4 vector4 = OtVec4(object);
		return OtVec4::create(vec4 / vector4->vec4);

	} else if (object.isKindOf<OtIntegerClass>() || object.isKindOf<OtRealClass>()) {
		return OtVec4::create(vec4 / object->operator float());

	} else {
		OtError("Expected a [Vec4], [Real] or [Integer] as the operand, not a [{}]", object->getType()->getName());
		return nullptr;
	}
}


//
//	OtVec4Class::length
//

float OtVec4Class::length() {
	return glm::length(vec4);
}


//
//	OtVec4Class::normalize
//

OtObject OtVec4Class::normalize() {
	return OtVec4::create(glm::normalize(vec4));
}


//
//	OtVec4Class::homogonize
//

OtObject OtVec4Class::homogonize() {
	return OtVec4::create(glm::vec4(vec4.x / vec4.w, vec4.y / vec4.w, vec4.z / vec4.w, 1.0f));
}


//
//	OtVec4Class::lerp
//

OtObject OtVec4Class::lerp(OtObject object, float delta) {
	if (object.isKindOf<OtVec4Class>()) {
		if (delta < 0.0f) {
			return OtVec4(this);

		} else if (delta > 1.0f) {
			return object;

		} else {
			OtVec4 vector4 = OtVec4(object);
			return OtVec4::create(vec4 + ((vector4->vec4 - vec4) * delta));
		}

	} else {
		OtError("Expected a [Vec4] as the operand, not a [{}]", object->getType()->getName());
		return nullptr;
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
