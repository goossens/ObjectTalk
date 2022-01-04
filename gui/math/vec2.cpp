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

#include "vec2.h"


//
//	OtVec2Class::init
//

OtObject OtVec2Class::init(size_t count, OtObject* parameters) {
	if (count == 2) {
		vector = glm::vec2(
			parameters[0]->operator float(),
			parameters[1]->operator float());

	} else if (count == 1) {
		vector = glm::vec2(parameters[0]->operator float());

	} else if (count != 0) {
		OtExcept("[Vec2] constructor expects 0, 1 or 2 arguments (not %ld)", count);
	}

	return nullptr;
}


//
//	OtVec2Class::add
//

OtObject OtVec2Class::add(OtObject object) {
	if (object->isKindOf("Vec2")) {
		OtVec2 vec2 = object->cast<OtVec2Class>();
		return create(vector + vec2->vector);

	} else if (object->isKindOf("Real")) {
		return create(vector + object->operator float());

	} else if (object->isKindOf("Integer")) {
		return create(vector + object->operator float());

	} else {
		OtExcept("Exected a [Vec2], [Real] or [Integer] as the operand, not a [%s]", object->getType()->getName().c_str());
		return nullptr;
	}
}


//
//	OtVec2Class::subtract
//

OtObject OtVec2Class::subtract(OtObject object) {
	if (object->isKindOf("Vec2")) {
		OtVec2 vec2 = object->cast<OtVec2Class>();
		return create(vector - vec2->vector);

	} else if (object->isKindOf("Real")) {
		return create(vector - object->operator float());

	} else if (object->isKindOf("Integer")) {
		return create(vector - object->operator float());

	} else {
		OtExcept("Exected a [Vec2], [Real] or [Integer] as the operand, not a [%s]", object->getType()->getName().c_str());
		return nullptr;
	}
}


//
//	OtVec2Class::multiply
//

OtObject OtVec2Class::multiply(OtObject object) {
	if (object->isKindOf("Vec2")) {
		OtVec2 vec2 = object->cast<OtVec2Class>();
		return create(vector * vec2->vector);

	} else if (object->isKindOf("Real")) {
		return create(vector * object->operator float());

	} else if (object->isKindOf("Integer")) {
		return create(vector * object->operator float());

	} else {
		OtExcept("Exected a [Vec2], [Real] or [Integer] as the operand, not a [%s]", object->getType()->getName().c_str());
		return nullptr;
	}
}


//
//	OtVec2Class::divide
//

OtObject OtVec2Class::divide(OtObject object) {
	if (object->isKindOf("Vec2")) {
		OtVec2 vec2 = object->cast<OtVec2Class>();
		return create(vector / vec2->vector);

	} else if (object->isKindOf("Real")) {
		return create(vector / object->operator float());

	} else if (object->isKindOf("Integer")) {
		return create(vector / object->operator float());

	} else {
		OtExcept("Exected a [Vec2], [Real] or [Integer] as the operand, not a [%s]", object->getType()->getName().c_str());
		return nullptr;
	}
}


//
//	OtVec2Class::getMeta
//

OtType OtVec2Class::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtVec2Class>("Vec2", OtMathClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtVec2Class::init));

		type->set("setX", OtFunctionClass::create(&OtVec2Class::setX));
		type->set("setY", OtFunctionClass::create(&OtVec2Class::setY));

		type->set("getX", OtFunctionClass::create(&OtVec2Class::getX));
		type->set("getY", OtFunctionClass::create(&OtVec2Class::getY));

		type->set("__add__", OtFunctionClass::create(&OtVec2Class::add));
		type->set("__sub__", OtFunctionClass::create(&OtVec2Class::subtract));
		type->set("__mul__", OtFunctionClass::create(&OtVec2Class::multiply));
		type->set("__div__", OtFunctionClass::create(&OtVec2Class::divide));

		type->set("length", OtFunctionClass::create(&OtVec2Class::length));
		type->set("normalize", OtFunctionClass::create(&OtVec2Class::normalize));
	}

	return type;
}


//
//	OtVec2Class::create
//

OtVec2 OtVec2Class::create() {
	OtVec2 vec2 = std::make_shared<OtVec2Class>();
	vec2->setType(getMeta());
	return vec2;
}

OtVec2 OtVec2Class::create(glm::vec2 vector) {
	OtVec2 vec2 = create();
	vec2->vector = vector;
	return vec2;
}

OtVec2 OtVec2Class::create(float x, float y) {
	OtVec2 vec2 = create();
	vec2->vector = glm::vec2(x, y);
	return vec2;
}
