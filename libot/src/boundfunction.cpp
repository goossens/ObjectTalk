//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
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

OtObject OtBoundFunctionClass::operator()(size_t count, OtObject* parameters) {
	// open a new stack frame
	OtVM::stack->openFrame(count + 1);

	// get a new stack pointer
	auto sp = OtVM::stack->sp(count + 1);

	// ugly patch
	*sp = object;

	// execute bound function with one more parameter (this)
	auto result = function->operator()(count + 1, sp);

	// close the stack frame
	OtVM::stack->closeFrame();

	// return bound function result
	return result;
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
