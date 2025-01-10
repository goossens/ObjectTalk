//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
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
		type = OtType::create<OtIntegerClass>("Integer", OtPrimitiveClass::getMeta());

		type->set("boolean", OtFunction::create(&OtIntegerClass::operator bool));
		type->set("integer", OtFunction::create(&OtIntegerClass::operator int64_t));
		type->set("real", OtFunction::create(&OtIntegerClass::operator double));
		type->set("string", OtFunction::create(&OtIntegerClass::operator std::string));

		type->set("__eq__", OtFunction::create(&OtIntegerClass::equal));
		type->set("__ne__", OtFunction::create(&OtIntegerClass::notEqual));
		type->set("__gt__", OtFunction::create(&OtIntegerClass::greaterThan));
		type->set("__lt__", OtFunction::create(&OtIntegerClass::lessThan));
		type->set("__ge__", OtFunction::create(&OtIntegerClass::greaterEqual));
		type->set("__le__", OtFunction::create(&OtIntegerClass::lessEqual));

		type->set("__add__", OtFunction::create(&OtIntegerClass::add));
		type->set("__sub__", OtFunction::create(&OtIntegerClass::subtract));
		type->set("__mul__", OtFunction::create(&OtIntegerClass::multiply));
		type->set("__div__", OtFunction::create(&OtIntegerClass::divide));
		type->set("__mod__", OtFunction::create(&OtIntegerClass::modulo));
		type->set("__pow__", OtFunction::create(&OtIntegerClass::power));

		type->set("__inc__", OtFunction::create(&OtIntegerClass::increment));
		type->set("__dec__", OtFunction::create(&OtIntegerClass::decrement));

		type->set("__lshift__", OtFunction::create(&OtIntegerClass::shiftLeft));
		type->set("__rshift__", OtFunction::create(&OtIntegerClass::shiftRight));

		type->set("__band__", OtFunction::create(&OtIntegerClass::bitwiseAnd));
		type->set("__bor__", OtFunction::create(&OtIntegerClass::bitwiseOr));
		type->set("__bxor__", OtFunction::create(&OtIntegerClass::bitwiseXor));
		type->set("__bnot__", OtFunction::create(&OtIntegerClass::bitwiseNot));

		type->set("__neg__", OtFunction::create(&OtIntegerClass::negate));
		type->set("__plus__", OtFunction::create(&OtIntegerClass::positive));

		type->set("abs", OtFunction::create(&OtIntegerClass::abs));
		type->set("sign", OtFunction::create(&OtIntegerClass::sign));
		type->set("min", OtFunction::create(&OtIntegerClass::min));
		type->set("max", OtFunction::create(&OtIntegerClass::max));
		type->set("clamp", OtFunction::create(&OtIntegerClass::clamp));
		type->set("random", OtFunction::create(&OtIntegerClass::random));
	}

	return type;
}
