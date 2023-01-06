//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtException.h"
#include "OtFunction.h"


//
//	OtFunctionClass::operator()
//

OtObject OtFunctionClass::operator () (size_t count, OtObject* parameters) {
	// sanity check
	if (parameterCount != SIZE_MAX && count != parameterCount) {
		if (parameterCount == 1) {
			OtExcept("Function expects [1] parameter, [%d] given", count);

		} else {
			OtExcept("Function expects [%d] parameters, [%d] given", parameterCount, count);
		}
	}

	return (*invoker)(target, count, parameters);
}


//
//	OtFunctionClass::getMeta()
//

OtType OtFunctionClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtFunctionClass>("Function", OtPrimitiveClass::getMeta());
		type->set("__call__", OtFunctionClass::create(&OtFunctionClass::operator()));
	}

	return type;
}
