//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/boundfunction.h"
#include "ot/function.h"
#include "ot/stack.h"
#include "ot/vm.h"



//
//	OtBoundFunctionClass::operator()
//

OtObject OtBoundFunctionClass::operator () (size_t count, OtObject* parameters) {
	return OtVM::redirectMemberFunction(object, function, count);
}


//
//	OtBoundFunctionClass::getMeta
//

OtType OtBoundFunctionClass::getMeta() {
	static OtType type = nullptr;

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
