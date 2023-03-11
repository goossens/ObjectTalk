//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
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

#include "OtVec3.h"


//
//	OtVec3Class::init
//

void OtVec3Class::init(size_t count, OtObject* parameters) {
	if (count == 3) {
		vec3 = glm::vec3(
			parameters[0]->operator float(),
			parameters[1]->operator float(),
			parameters[2]->operator float());

	} else if (count == 1) {
		vec3 = glm::vec3(parameters[0]->operator float());

	} else if (count != 0) {
		OtExcept("[Vec3] constructor expects 0, 1 or 3 arguments (not %ld)", count);
	}
}


//
//	OtVec3Class::add
//

OtObject OtVec3Class::add(OtObject object) {
	if (object.isKindOf<OtVec3Class>()) {
		OtVec3 vector3 = OtVec3(object);
		return OtVec3::create(vec3 + vector3->vec3);

	} else if (object.isKindOf<OtIntegerClass>() || object.isKindOf<OtRealClass>()) {
		return OtVec3::create(vec3 + object->operator float());

	} else {
		OtExcept("Expected a [Vec3], [Real] or [Integer] as the operand, not a [%s]", object->getType()->getName().c_str());
		return nullptr;
	}
}


//
//	OtVec3Class::subtract
//

OtObject OtVec3Class::subtract(OtObject object) {
	if (object.isKindOf<OtVec3Class>()) {
		OtVec3 vector3 = OtVec3(object);
		return OtVec3::create(vec3 - vector3->vec3);

	} else if (object.isKindOf<OtIntegerClass>() || object.isKindOf<OtRealClass>()) {
		return OtVec3::create(vec3 - object->operator float());

	} else {
		OtExcept("Expected a [Vec3], [Real] or [Integer] as the operand, not a [%s]", object->getType()->getName().c_str());
		return nullptr;
	}
}


//
//	OtVec3Class::multiply
//

OtObject OtVec3Class::multiply(OtObject object) {
	if (object.isKindOf<OtVec3Class>()) {
		OtVec3 vector3 = OtVec3(object);
		return OtVec3::create(vec3 * vector3->vec3);

	} else if (object.isKindOf<OtIntegerClass>() || object.isKindOf<OtRealClass>()) {
		return OtVec3::create(vec3 * object->operator float());

	} else {
		OtExcept("Expected a [Vec3], [Real] or [Integer] as the operand, not a [%s]", object->getType()->getName().c_str());
		return nullptr;
	}
}


//
//	OtVec3Class::divide
//

OtObject OtVec3Class::divide(OtObject object) {
	if (object.isKindOf<OtVec3Class>()) {
		OtVec3 vector3 = OtVec3(object);
		return OtVec3::create(vec3 / vector3->vec3);

	} else if (object.isKindOf<OtIntegerClass>() || object.isKindOf<OtRealClass>()) {
		return OtVec3::create(vec3 / object->operator float());

	} else {
		OtExcept("Expected a [Vec3], [Real] or [Integer] as the operand, not a [%s]", object->getType()->getName().c_str());
		return nullptr;
	}
}


//
//	OtVec3Class::length
//

float OtVec3Class::length() {
	return glm::length(vec3);
}


//
//	OtVec3Class::normalize
//

OtObject OtVec3Class::normalize() {
	return OtVec3::create(glm::normalize(vec3));
}


//
//	OtVec3Class::distance
//

float OtVec3Class::distance(OtObject object) {
	if (object.isKindOf<OtVec3Class>()) {
		OtVec3 vector3 = OtVec3(object);
		return glm::distance(vec3, vector3->vec3);

	} else {
		OtExcept("Expected a [Vec3] as the operand, not a [%s]", object->getType()->getName().c_str());
		return 0.0f;
	}

}


//
//	OtVec3Class::dot
//

float OtVec3Class::dot(OtObject object) {
	if (object.isKindOf<OtVec3Class>()) {
		OtVec3 vector3 = OtVec3(object);
		return glm::dot(vec3, vector3->vec3);

	} else {
		OtExcept("Expected a [Vec3] as the operand, not a [%s]", object->getType()->getName().c_str());
		return 0.0f;
	}
}


//
//	OtVec3Class::cross
//

OtObject OtVec3Class::cross(OtObject object) {
	if (object.isKindOf<OtVec3Class>()) {
		OtVec3 vector3 = OtVec3(object);
		return OtVec3::create(glm::cross(vec3, vector3->vec3));

	} else {
		OtExcept("Expected a [Vec3] as the operand, not a [%s]", object->getType()->getName().c_str());
		return nullptr;
	}
}


//
//	OtVec3Class::lerp
//

OtObject OtVec3Class::lerp(OtObject object, float delta) {
	if (object.isKindOf<OtVec3Class>()) {
		if (delta < 0.0f) {
			return OtVec3(this);

		} else if (delta > 1.0f) {
			return object;

		} else {
			OtVec3 vector3 = OtVec3(object);
			return OtVec3::create(vec3 + ((vector3->vec3 - vec3) * delta));
		}

	} else {
		OtExcept("Expected a [Vec3] as the operand, not a [%s]", object->getType()->getName().c_str());
		return nullptr;
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

		type->set("length", OtFunction::create(&OtVec3Class::length));
		type->set("normalize", OtFunction::create(&OtVec3Class::normalize));
		type->set("distance", OtFunction::create(&OtVec3Class::distance));
		type->set("dot", OtFunction::create(&OtVec3Class::dot));
		type->set("cross", OtFunction::create(&OtVec3Class::cross));
		type->set("lerp", OtFunction::create(&OtVec3Class::lerp));
	}

	return type;
}
