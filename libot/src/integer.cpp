//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/integer.h"
#include "ot/function.h"


//
//	OtIntegerClass::getMeta
//

OtType OtIntegerClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtIntegerClass>("Integer", OtPrimitiveClass::getMeta());

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

		type->set("__eq__", OtFunctionClass::create(&OtIntegerClass::equal));
		type->set("__ne__", OtFunctionClass::create(&OtIntegerClass::notEqual));
		type->set("__gt__", OtFunctionClass::create(&OtIntegerClass::greaterThan));
		type->set("__lt__", OtFunctionClass::create(&OtIntegerClass::lessThan));
		type->set("__ge__", OtFunctionClass::create(&OtIntegerClass::greaterEqual));
		type->set("__le__", OtFunctionClass::create(&OtIntegerClass::lessEqual));

		type->set("__neg__", OtFunctionClass::create(&OtIntegerClass::negate));

		type->set("abs", OtFunctionClass::create(&OtIntegerClass::abs));
		type->set("sign", OtFunctionClass::create(&OtIntegerClass::sign));
	}

	return type;
}


//
//	OtIntegerClass::create
//

OtInteger OtIntegerClass::create(long value) {
	OtInteger integer = std::make_shared<OtIntegerClass>(value);
	integer->setType(getMeta());
	return integer;
}
