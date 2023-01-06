//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
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

OtObject OtBoundFunctionClass::operator () (size_t count, OtObject* parameters) {
	return OtVM::instance()->redirectMemberFunction(object, function, count);
}


//
//	OtBoundFunctionClass::getMeta
//

OtType OtBoundFunctionClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtBoundFunctionClass>("BoundFunction", OtInternalClass::getMeta());
		type->set("__call__", OtFunctionClass::create(&OtBoundFunctionClass::operator()));
	}

	return type;
}


//
//	OtBoundFunctionClass::create
//

OtBoundFunction OtBoundFunctionClass::create(OtObject o, OtObject f) {
	OtBoundFunction bound = std::make_shared<OtBoundFunctionClass>(o, f);
	bound->setType(getMeta());
	return bound;
}
