//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtBoundFunction.h"
#include "OtFunction.h"
#include "OtStack.h"
#include "OtVM.h"


//
//	OtBoundFunctionClass::operator()
//

OtObject OtBoundFunctionClass::operator()(size_t count, OtObject* parameters) {
	return OtVM::redirectMemberFunction(object, function, count);
}


//
//	OtBoundFunctionClass::getMeta
//

OtType OtBoundFunctionClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtBoundFunctionClass>("BoundFunction", OtInternalClass::getMeta());
		type->set("__call__", OtFunction::create(&OtBoundFunctionClass::operator()));
	}

	return type;
}
