//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <sstream>
#include <iomanip>

#include "OtFunction.h"
#include "OtReal.h"


//
//	OtRealClass::toFixed
//

std::string OtRealClass::toFixed(int precision) {
	std::ostringstream stream;
	stream << std::fixed << std::setprecision(precision) << value;
	return stream.str();
}


//
//	OtRealClass::getMeta
//

OtType OtRealClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtRealClass>("Real", OtPrimitiveClass::getMeta());

		type->set("boolean", OtFunction::create(&OtRealClass::operator bool));
		type->set("integer", OtFunction::create(&OtRealClass::operator int64_t));
		type->set("real", OtFunction::create(&OtRealClass::operator double));
		type->set("string", OtFunction::create(&OtRealClass::operator std::string));

		type->set("__eq__", OtFunction::create(&OtRealClass::equal));
		type->set("__ne__", OtFunction::create(&OtRealClass::notEqual));
		type->set("__gt__", OtFunction::create(&OtRealClass::greaterThan));
		type->set("__lt__", OtFunction::create(&OtRealClass::lessThan));
		type->set("__ge__", OtFunction::create(&OtRealClass::greaterEqual));
		type->set("__le__", OtFunction::create(&OtRealClass::lessEqual));

		type->set("__add__", OtFunction::create(&OtRealClass::add));
		type->set("__sub__", OtFunction::create(&OtRealClass::subtract));
		type->set("__mul__", OtFunction::create(&OtRealClass::multiply));
		type->set("__div__", OtFunction::create(&OtRealClass::divide));
		type->set("__mod__", OtFunction::create(&OtRealClass::modulo));
		type->set("__pow__", OtFunction::create(&OtRealClass::power));

		type->set("__neg__", OtFunction::create(&OtRealClass::negate));
		type->set("__plus__", OtFunction::create(&OtRealClass::positive));

		type->set("abs", OtFunction::create(&OtRealClass::abs));
		type->set("sign", OtFunction::create(&OtRealClass::sign));
		type->set("round", OtFunction::create(&OtRealClass::round));
		type->set("ceil", OtFunction::create(&OtRealClass::ceil));
		type->set("floor", OtFunction::create(&OtRealClass::floor));
		type->set("trunc", OtFunction::create(&OtRealClass::trunc));
		type->set("fraction", OtFunction::create(&OtRealClass::fraction));
		type->set("min", OtFunction::create(&OtRealClass::min));
		type->set("max", OtFunction::create(&OtRealClass::max));
		type->set("clamp", OtFunction::create(&OtRealClass::clamp));
		type->set("lerp", OtFunction::create(&OtRealClass::lerp));

		type->set("sin", OtFunction::create(&OtRealClass::sin));
		type->set("cos", OtFunction::create(&OtRealClass::cos));
		type->set("tan", OtFunction::create(&OtRealClass::tan));
		type->set("asin", OtFunction::create(&OtRealClass::asin));
		type->set("acos", OtFunction::create(&OtRealClass::acos));
		type->set("atan", OtFunction::create(&OtRealClass::atan));
		type->set("atan2", OtFunction::create(&OtRealClass::atan2));
		type->set("radians", OtFunction::create(&OtRealClass::radians));
		type->set("degrees", OtFunction::create(&OtRealClass::degrees));

		type->set("sqrt", OtFunction::create(&OtRealClass::sqrt));
		type->set("pow", OtFunction::create(&OtRealClass::pow));

		type->set("log", OtFunction::create(&OtRealClass::log));
		type->set("exp", OtFunction::create(&OtRealClass::exp));
		type->set("log10", OtFunction::create(&OtRealClass::log10));

		type->set("random", OtFunction::create(&OtRealClass::random));

		type->set("toFixed", OtFunction::create(&OtRealClass::toFixed));
	}

	return type;
}
