//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/exception.h"
#include "ot/function.h"

#include "vector3.h"


//
//	OtVector3Class::init
//

OtObject OtVector3Class::init(size_t count, OtObject* parameters) {
	if (count == 3) {
		vec3 = glm::vec3(
			parameters[0]->operator float(),
			parameters[1]->operator float(),
			parameters[2]->operator float());

	} else if (count == 1) {
		vec3 = glm::vec3(parameters[0]->operator float());

	} else if (count != 0) {
		OtExcept("[Vector3] constructor expects 0, 1 or 3 arguments (not %ld)", count);
	}

	return nullptr;
}


//
//	OtVector3Class::add
//

OtObject OtVector3Class::add(OtObject object) {
	if (object->isKindOf("Vector3")) {
		OtVector3 vector3 = object->cast<OtVector3Class>();
		return create(vec3 + vector3->vec3);

	} else if (object->isKindOf("Real")) {
		return create(vec3 + object->operator float());

	} else if (object->isKindOf("Integer")) {
		return create(vec3 + object->operator float());

	} else {
		OtExcept("Exected a [Vector3], [Real] or [Integer] as the operand, not a [%s]", object->getType()->getName().c_str());
		return nullptr;
	}
}


//
//	OtVector3Class::subtract
//

OtObject OtVector3Class::subtract(OtObject object) {
	if (object->isKindOf("Vector3")) {
		OtVector3 vector3 = object->cast<OtVector3Class>();
		return create(vec3 - vector3->vec3);

	} else if (object->isKindOf("Real")) {
		return create(vec3 - object->operator float());

	} else if (object->isKindOf("Integer")) {
		return create(vec3 - object->operator float());

	} else {
		OtExcept("Exected a [Vector3], [Real] or [Integer] as the operand, not a [%s]", object->getType()->getName().c_str());
		return nullptr;
	}
}


//
//	OtVector3Class::multiply
//

OtObject OtVector3Class::multiply(OtObject object) {
	if (object->isKindOf("Vector3")) {
		OtVector3 vector3 = object->cast<OtVector3Class>();
		return create(vec3 * vector3->vec3);

	} else if (object->isKindOf("Real")) {
		return create(vec3 * object->operator float());

	} else if (object->isKindOf("Integer")) {
		return create(vec3 * object->operator float());

	} else {
		OtExcept("Exected a [Vector3], [Real] or [Integer] as the operand, not a [%s]", object->getType()->getName().c_str());
		return nullptr;
	}
}


//
//	OtVector3Class::divide
//

OtObject OtVector3Class::divide(OtObject object) {
	if (object->isKindOf("Vector3")) {
		OtVector3 vector3 = object->cast<OtVector3Class>();
		return create(vec3 / vector3->vec3);

	} else if (object->isKindOf("Real")) {
		return create(vec3 / object->operator float());

	} else if (object->isKindOf("Integer")) {
		return create(vec3 / object->operator float());

	} else {
		OtExcept("Exected a [Vector3], [Real] or [Integer] as the operand, not a [%s]", object->getType()->getName().c_str());
		return nullptr;
	}
}


//
//	OtVector3Class::getMeta
//

OtType OtVector3Class::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtVector3Class>("Vector3", OtMathClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtVector3Class::init));

		type->set("setX", OtFunctionClass::create(&OtVector3Class::setX));
		type->set("setY", OtFunctionClass::create(&OtVector3Class::setY));
		type->set("setZ", OtFunctionClass::create(&OtVector3Class::setZ));

		type->set("getX", OtFunctionClass::create(&OtVector3Class::getX));
		type->set("getY", OtFunctionClass::create(&OtVector3Class::getY));
		type->set("getZ", OtFunctionClass::create(&OtVector3Class::getZ));

		type->set("__add__", OtFunctionClass::create(&OtVector3Class::add));
		type->set("__sub__", OtFunctionClass::create(&OtVector3Class::subtract));
		type->set("__mul__", OtFunctionClass::create(&OtVector3Class::multiply));
		type->set("__div__", OtFunctionClass::create(&OtVector3Class::divide));
	}

	return type;
}


//
//	OtVector3Class::create
//

OtVector3 OtVector3Class::create() {
	OtVector3 vector3 = std::make_shared<OtVector3Class>();
	vector3->setType(getMeta());
	return vector3;
}

OtVector3 OtVector3Class::create(glm::vec3 vec3) {
	OtVector3 vector3 = create();
	vector3->vec3 = vec3;
	return vector3;
}

OtVector3 OtVector3Class::create(float x, float y, float z) {
	OtVector3 vector3 = create();
	vector3->vec3 = glm::vec3(x, y, z);
	return vector3;
}
