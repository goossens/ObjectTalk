//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtException.h"
#include "OtFunction.h"
#include "OtClosure.h"
#include "OtVM.h"


//
//	OtClosureClass::capture
//

OtObject OtClosureClass::capture() {
	// create a clone to capture variables
	auto clone = OtClosure::create(function, captures);

	// setup all captured variables
	for (auto const& capture : captures) {
		clone->set(capture.first, OtVM::getStack()->getFrameItem(capture.second.first, capture.second.second));
	}

	// return a cloned closure with the captured variables
	return clone;
}


//
//	OtClosureClass::operator()
//

OtObject OtClosureClass::operator()(size_t count, OtObject* parameters) {
	// register closure on the stack
	auto stack = OtVM::getStack();
	stack->pushClosure(OtClosure(this));

	// execute the enclosed function
	auto result = function->operator()(count, parameters);

	// remove this closure from the stack
	stack->popClosure();

	// the result of the function is our result
	return result;
}


//
//	OtClosureClass::getMeta
//

OtType OtClosureClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtClosureClass>("Closure", OtInternalClass::getMeta());
		type->set("__capture__", OtFunction::create(&OtClosureClass::capture));
		type->set("__call__", OtFunction::create(&OtClosureClass::operator()));
	}

	return type;
}
