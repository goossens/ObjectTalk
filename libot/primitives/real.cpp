//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <sstream>
#include <iomanip>

#include "ot/real.h"
#include "ot/function.h"


//
//	OtRealClass::toFixed
//

std::string OtRealClass::toFixed(size_t precision) {
	std::ostringstream stream;
	stream << std::fixed << std::setprecision(precision) << value;
	return stream.str();
}


//
//	OtRealClass::getMeta
//

OtType OtRealClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtRealClass>("Real", OtPrimitiveClass::getMeta());

		type->set("boolean", OtFunctionClass::create(&OtRealClass::operator bool));
		type->set("integer", OtFunctionClass::create(&OtRealClass::operator long));
		type->set("real", OtFunctionClass::create(&OtRealClass::operator double));
		type->set("string", OtFunctionClass::create(&OtRealClass::operator std::string));

		type->set("__eq__", OtFunctionClass::create(&OtRealClass::equal));
		type->set("__ne__", OtFunctionClass::create(&OtRealClass::notEqual));
		type->set("__gt__", OtFunctionClass::create(&OtRealClass::greaterThan));
		type->set("__lt__", OtFunctionClass::create(&OtRealClass::lessThan));
		type->set("__ge__", OtFunctionClass::create(&OtRealClass::greaterEqual));
		type->set("__le__", OtFunctionClass::create(&OtRealClass::lessEqual));

		type->set("__add__", OtFunctionClass::create(&OtRealClass::add));
		type->set("__sub__", OtFunctionClass::create(&OtRealClass::subtract));
		type->set("__mul__", OtFunctionClass::create(&OtRealClass::multiply));
		type->set("__div__", OtFunctionClass::create(&OtRealClass::divide));
		type->set("__mod__", OtFunctionClass::create(&OtRealClass::modulo));
		type->set("__pow__", OtFunctionClass::create(&OtRealClass::power));

		type->set("__neg__", OtFunctionClass::create(&OtRealClass::negate));
		type->set("__plus__", OtFunctionClass::create(&OtRealClass::positive));

		type->set("abs", OtFunctionClass::create(&OtRealClass::abs));
		type->set("sign", OtFunctionClass::create(&OtRealClass::sign));
		type->set("round", OtFunctionClass::create(&OtRealClass::round));
		type->set("ceil", OtFunctionClass::create(&OtRealClass::ceil));
		type->set("floor", OtFunctionClass::create(&OtRealClass::floor));
		type->set("trunc", OtFunctionClass::create(&OtRealClass::trunc));
		type->set("min", OtFunctionClass::create(&OtRealClass::min));
		type->set("max", OtFunctionClass::create(&OtRealClass::max));
		type->set("clamp", OtFunctionClass::create(&OtRealClass::clamp));

		type->set("sin", OtFunctionClass::create(&OtRealClass::sin));
		type->set("cos", OtFunctionClass::create(&OtRealClass::cos));
		type->set("tan", OtFunctionClass::create(&OtRealClass::tan));
		type->set("asin", OtFunctionClass::create(&OtRealClass::asin));
		type->set("acos", OtFunctionClass::create(&OtRealClass::acos));
		type->set("atan", OtFunctionClass::create(&OtRealClass::atan));
		type->set("radians", OtFunctionClass::create(&OtRealClass::radians));
		type->set("degrees", OtFunctionClass::create(&OtRealClass::degrees));

		type->set("sqrt", OtFunctionClass::create(&OtRealClass::sqrt));
		type->set("pow", OtFunctionClass::create(&OtRealClass::pow));

		type->set("log", OtFunctionClass::create(&OtRealClass::log));
		type->set("exp", OtFunctionClass::create(&OtRealClass::exp));
		type->set("log10", OtFunctionClass::create(&OtRealClass::log10));

		type->set("random", OtFunctionClass::create(&OtRealClass::random));

		type->set("toFixed", OtFunctionClass::create(&OtRealClass::toFixed));
	}

	return type;
}


//
//	OtRealClass::create
//

OtReal OtRealClass::create(double value) {
	OtReal real = std::make_shared<OtRealClass>(value);
	real->setType(getMeta());
	return real;
}
