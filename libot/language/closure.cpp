//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/exception.h"
#include "ot/function.h"
#include "ot/closure.h"
#include "ot/vm.h"


//
//	OtClosureClass::capture
//

OtObject OtClosureClass::capture() {
	// create a clone to capture variables
	auto clone = OtClosureClass::create(function, captures);

	// setup all captured variables
	for (auto const& capture : captures) {
		clone->set(capture.first, OtVM::getStack()->getFrameItem(capture.second));
	}

	// return a cloned closure with the captured variables
	return clone;
}


//
//	OtClosureClass::operator()
//

OtObject OtClosureClass::operator () (size_t count, OtObject* parameters) {
	// register closure on the stack
	OtVM::getStack()->pushClosure(shared());

	// execute the enclosed function
	auto result = function->operator () (count, parameters);

	// remove this closure from the stack
	OtVM::getStack()->popClosure();

	// the result of the function is our result
	return result;
}


//
//	OtClosureClass::getMeta
//

OtType OtClosureClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtClosureClass>("Closure", OtInternalClass::getMeta());
		type->set("__capture__", OtFunctionClass::create(&OtClosureClass::capture));
		type->set("__call__", OtFunctionClass::create(&OtClosureClass::operator()));
	}

	return type;
}


//
//	OtClosureClass::create
//

OtClosure OtClosureClass::create(OtByteCodeFunction function, const std::unordered_map<std::string, OtStackItem>& captures) {
	OtClosure closure = std::make_shared<OtClosureClass>(function, captures);
	closure->setType(getMeta());
	return closure;
}
