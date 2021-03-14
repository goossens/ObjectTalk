//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/real.h"
#include "ot/function.h"


//
//	OtRealClass::getMeta
//

OtType OtRealClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtRealClass>("Real", OtPrimitiveClass::getMeta());

		type->set("__add__", OtFunctionClass::create(&OtRealClass::add));
		type->set("__sub__", OtFunctionClass::create(&OtRealClass::subtract));
		type->set("__mul__", OtFunctionClass::create(&OtRealClass::multiply));
		type->set("__div__", OtFunctionClass::create(&OtRealClass::divide));
		type->set("__pow__", OtFunctionClass::create(&OtRealClass::power));

		type->set("__eq__", OtFunctionClass::create(&OtRealClass::equal));
		type->set("__ne__", OtFunctionClass::create(&OtRealClass::notEqual));
		type->set("__gt__", OtFunctionClass::create(&OtRealClass::greaterThan));
		type->set("__lt__", OtFunctionClass::create(&OtRealClass::lessThan));
		type->set("__ge__", OtFunctionClass::create(&OtRealClass::greaterEqual));
		type->set("__le__", OtFunctionClass::create(&OtRealClass::lessEqual));

		type->set("__neg__", OtFunctionClass::create(&OtRealClass::negate));

		type->set("abs", OtFunctionClass::create(&OtRealClass::abs));
		type->set("sign", OtFunctionClass::create(&OtRealClass::sign));
		type->set("round", OtFunctionClass::create(&OtRealClass::round));
		type->set("ceil", OtFunctionClass::create(&OtRealClass::ceil));
		type->set("floor", OtFunctionClass::create(&OtRealClass::floor));
		type->set("sin", OtFunctionClass::create(&OtRealClass::sin));
		type->set("cos", OtFunctionClass::create(&OtRealClass::cos));
		type->set("tan", OtFunctionClass::create(&OtRealClass::tan));
		type->set("radians", OtFunctionClass::create(&OtRealClass::radians));
		type->set("degrees", OtFunctionClass::create(&OtRealClass::degrees));
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
