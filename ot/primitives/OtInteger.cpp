//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtFunction.h"
#include "OtInteger.h"


//
//	OtIntegerClass::getMeta
//

OtType OtIntegerClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtIntegerClass>("Integer", OtPrimitiveClass::getMeta());

		type->set("boolean", OtFunctionClass::create(&OtIntegerClass::operator bool));
		type->set("integer", OtFunctionClass::create(&OtIntegerClass::operator int64_t));
		type->set("real", OtFunctionClass::create(&OtIntegerClass::operator double));
		type->set("string", OtFunctionClass::create(&OtIntegerClass::operator std::string));

		type->set("__eq__", OtFunctionClass::create(&OtIntegerClass::equal));
		type->set("__ne__", OtFunctionClass::create(&OtIntegerClass::notEqual));
		type->set("__gt__", OtFunctionClass::create(&OtIntegerClass::greaterThan));
		type->set("__lt__", OtFunctionClass::create(&OtIntegerClass::lessThan));
		type->set("__ge__", OtFunctionClass::create(&OtIntegerClass::greaterEqual));
		type->set("__le__", OtFunctionClass::create(&OtIntegerClass::lessEqual));

		type->set("__add__", OtFunctionClass::create(&OtIntegerClass::add));
		type->set("__sub__", OtFunctionClass::create(&OtIntegerClass::subtract));
		type->set("__mul__", OtFunctionClass::create(&OtIntegerClass::multiply));
		type->set("__div__", OtFunctionClass::create(&OtIntegerClass::divide));
		type->set("__mod__", OtFunctionClass::create(&OtIntegerClass::modulo));
		type->set("__pow__", OtFunctionClass::create(&OtIntegerClass::power));

		type->set("__inc__", OtFunctionClass::create(&OtIntegerClass::increment));
		type->set("__dec__", OtFunctionClass::create(&OtIntegerClass::decrement));

		type->set("__lshift__", OtFunctionClass::create(&OtIntegerClass::shiftLeft));
		type->set("__rshift__", OtFunctionClass::create(&OtIntegerClass::shiftRight));

		type->set("__band__", OtFunctionClass::create(&OtIntegerClass::bitwiseAnd));
		type->set("__bor__", OtFunctionClass::create(&OtIntegerClass::bitwiseOr));
		type->set("__bxor__", OtFunctionClass::create(&OtIntegerClass::bitwiseXor));
		type->set("__bnot__", OtFunctionClass::create(&OtIntegerClass::bitwiseNot));

		type->set("__neg__", OtFunctionClass::create(&OtIntegerClass::negate));
		type->set("__plus__", OtFunctionClass::create(&OtIntegerClass::positive));

		type->set("abs", OtFunctionClass::create(&OtIntegerClass::abs));
		type->set("sign", OtFunctionClass::create(&OtIntegerClass::sign));
		type->set("min", OtFunctionClass::create(&OtIntegerClass::min));
		type->set("max", OtFunctionClass::create(&OtIntegerClass::max));
		type->set("clamp", OtFunctionClass::create(&OtIntegerClass::clamp));
		type->set("random", OtFunctionClass::create(&OtIntegerClass::random));
	}

	return type;
}


//
//	OtIntegerClass::create
//

OtInteger OtIntegerClass::create(int64_t value) {
	OtInteger integer = std::make_shared<OtIntegerClass>(value);
	integer->setType(getMeta());
	return integer;
}
