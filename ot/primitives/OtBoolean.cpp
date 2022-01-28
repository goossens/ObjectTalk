//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
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
		type = OtTypeClass::create<OtBooleanClass>("Boolean", OtPrimitiveClass::getMeta());

		type->set("boolean", OtFunctionClass::create(&OtBooleanClass::operator bool));
		type->set("integer", OtFunctionClass::create(&OtBooleanClass::operator long));
		type->set("real", OtFunctionClass::create(&OtBooleanClass::operator double));
		type->set("string", OtFunctionClass::create(&OtBooleanClass::operator std::string));

		type->set("__eq__", OtFunctionClass::create(&OtBooleanClass::equal));
		type->set("__ne__", OtFunctionClass::create(&OtBooleanClass::notEqual));

		type->set("__and__", OtFunctionClass::create(&OtBooleanClass::logicalAnd));
		type->set("__or__", OtFunctionClass::create(&OtBooleanClass::logicalOr));
		type->set("__not__", OtFunctionClass::create(&OtBooleanClass::logicalNot));
	}

	return type;
}

//
//	OtBooleanClass::create
//

OtBoolean OtBooleanClass::create(bool value) {
	OtBoolean boolean = std::make_shared<OtBooleanClass>(value);
	boolean->setType(getMeta());
	return boolean;
}
