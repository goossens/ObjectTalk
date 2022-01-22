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
#include "ot/bytecodefunction.h"
#include "ot/vm.h"


//
//	OtByteCodeFunctionClass::operator()
//

OtObject OtByteCodeFunctionClass::operator () (size_t count, OtObject* parameters) {
	// sanity check
	if (parameterCount != count) {
		if (parameterCount == 1) {
			OtExcept("Function expects [%d] parameter, [%d] given", parameterCount, count);

		} else {
			OtExcept("Function expects [%d] parameters, [%d] given", parameterCount, count);
		}
	}

	// execute function in VM
	auto result = OtVM::instance()->execute(bytecode, count);
	return result;
}


//
//	OtByteCodeFunctionClass::getMeta
//

OtType OtByteCodeFunctionClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtByteCodeFunctionClass>("ByteCodeFunction", OtInternalClass::getMeta());
		type->set("__call__", OtFunctionClass::create(&OtByteCodeFunctionClass::operator()));
	}

	return type;
}


//
//	OtByteCodeFunctionClass::create
//

OtByteCodeFunction OtByteCodeFunctionClass::create(OtByteCode bytecode, size_t parameterCount) {
	OtByteCodeFunction func = std::make_shared<OtByteCodeFunctionClass>(bytecode, parameterCount);
	func->setType(getMeta());
	return func;
}
