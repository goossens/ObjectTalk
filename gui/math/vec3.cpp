//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/exception.h"
#include "ot/function.h"

#include "vec3.h"


//
//	OtVec3Class::init
//

OtObject OtVec3Class::init(size_t count, OtObject* parameters) {
	if (count == 3) {
		vector = glm::vec3(
			parameters[0]->operator float(),
			parameters[1]->operator float(),
			parameters[2]->operator float());

	} else if (count == 1) {
		vector = glm::vec3(parameters[0]->operator float());

	} else if (count != 0) {
		OtExcept("[Vec3] constructor expects 0, 1 or 3 arguments (not %ld)", count);
	}

	return nullptr;
}


//
//	OtVec3Class::add
//

OtObject OtVec3Class::add(OtObject object) {
	if (object->isKindOf("Vec3")) {
		OtVec3 vec3 = object->cast<OtVec3Class>();
		return create(vector + vec3->vector);

	} else if (object->isKindOf("Real")) {
		return create(vector + object->operator float());

	} else if (object->isKindOf("Integer")) {
		return create(vector + object->operator float());

	} else {
		OtExcept("Exected a [Vec3], [Real] or [Integer] as the operand, not a [%s]", object->getType()->getName().c_str());
		return nullptr;
	}
}


//
//	OtVec3Class::subtract
//

OtObject OtVec3Class::subtract(OtObject object) {
	if (object->isKindOf("Vec3")) {
		OtVec3 vec3 = object->cast<OtVec3Class>();
		return create(vector - vec3->vector);

	} else if (object->isKindOf("Real")) {
		return create(vector - object->operator float());

	} else if (object->isKindOf("Integer")) {
		return create(vector - object->operator float());

	} else {
		OtExcept("Exected a [Vec3], [Real] or [Integer] as the operand, not a [%s]", object->getType()->getName().c_str());
		return nullptr;
	}
}


//
//	OtVec3Class::multiply
//

OtObject OtVec3Class::multiply(OtObject object) {
	if (object->isKindOf("Vec3")) {
		OtVec3 vec3 = object->cast<OtVec3Class>();
		return create(vector * vec3->vector);

	} else if (object->isKindOf("Real")) {
		return create(vector * object->operator float());

	} else if (object->isKindOf("Integer")) {
		return create(vector * object->operator float());

	} else {
		OtExcept("Exected a [Vec3], [Real] or [Integer] as the operand, not a [%s]", object->getType()->getName().c_str());
		return nullptr;
	}
}


//
//	OtVec3Class::divide
//

OtObject OtVec3Class::divide(OtObject object) {
	if (object->isKindOf("Vec3")) {
		OtVec3 vec3 = object->cast<OtVec3Class>();
		return create(vector / vec3->vector);

	} else if (object->isKindOf("Real")) {
		return create(vector / object->operator float());

	} else if (object->isKindOf("Integer")) {
		return create(vector / object->operator float());

	} else {
		OtExcept("Exected a [Vec3], [Real] or [Integer] as the operand, not a [%s]", object->getType()->getName().c_str());
		return nullptr;
	}
}


//
//	OtVec3Class::getMeta
//

OtType OtVec3Class::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtVec3Class>("Vec3", OtMathClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtVec3Class::init));

		type->set("setX", OtFunctionClass::create(&OtVec3Class::setX));
		type->set("setY", OtFunctionClass::create(&OtVec3Class::setY));
		type->set("setZ", OtFunctionClass::create(&OtVec3Class::setZ));

		type->set("getX", OtFunctionClass::create(&OtVec3Class::getX));
		type->set("getY", OtFunctionClass::create(&OtVec3Class::getY));
		type->set("getZ", OtFunctionClass::create(&OtVec3Class::getZ));

		type->set("__add__", OtFunctionClass::create(&OtVec3Class::add));
		type->set("__sub__", OtFunctionClass::create(&OtVec3Class::subtract));
		type->set("__mul__", OtFunctionClass::create(&OtVec3Class::multiply));
		type->set("__div__", OtFunctionClass::create(&OtVec3Class::divide));

		type->set("length", OtFunctionClass::create(&OtVec3Class::length));
		type->set("normalize", OtFunctionClass::create(&OtVec3Class::normalize));
	}

	return type;
}


//
//	OtVec3Class::create
//

OtVec3 OtVec3Class::create() {
	OtVec3 vec3 = std::make_shared<OtVec3Class>();
	vec3->setType(getMeta());
	return vec3;
}

OtVec3 OtVec3Class::create(glm::vec3 vector) {
	OtVec3 vec3 = create();
	vec3->vector = vector;
	return vec3;
}

OtVec3 OtVec3Class::create(float x, float y, float z) {
	OtVec3 vec3 = create();
	vec3->vector = glm::vec3(x, y, z);
	return vec3;
}
