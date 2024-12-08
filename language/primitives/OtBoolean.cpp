//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtBoolean.h"
#include "OtFunction.h"


//
//	OtBooleanClass::getMeta
//

OtType OtBooleanClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtBooleanClass>("Boolean", OtPrimitiveClass::getMeta());

		type->set("boolean", OtFunction::create(&OtBooleanClass::operator bool));
		type->set("integer", OtFunction::create(&OtBooleanClass::operator int64_t));
		type->set("real", OtFunction::create(&OtBooleanClass::operator double));
		type->set("string", OtFunction::create(&OtBooleanClass::operator std::string));

		type->set("__eq__", OtFunction::create(&OtBooleanClass::equal));
		type->set("__ne__", OtFunction::create(&OtBooleanClass::notEqual));

		type->set("__and__", OtFunction::create(&OtBooleanClass::logicalAnd));
		type->set("__or__", OtFunction::create(&OtBooleanClass::logicalOr));
		type->set("__not__", OtFunction::create(&OtBooleanClass::logicalNot));

		type->set("__band__", OtFunction::create(&OtBooleanClass::bitwiseAnd));
		type->set("__bor__", OtFunction::create(&OtBooleanClass::bitwiseOr));
		type->set("__bxor__", OtFunction::create(&OtBooleanClass::bitwiseXor));
		type->set("__bnot__", OtFunction::create(&OtBooleanClass::bitwiseNot));
	}

	return type;
}
