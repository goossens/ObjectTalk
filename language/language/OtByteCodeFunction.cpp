//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtByteCodeFunction.h"
#include "OtFunction.h"
#include "OtLog.h"
#include "OtVM.h"


//
//	OtByteCodeFunctionClass::operator()
//

OtObject OtByteCodeFunctionClass::operator()(size_t count, OtObject* /* parameters */) {
	// sanity check
	if (parameterCount != count) {
		if (parameterCount == 1) {
			OtLogError("Function expects [{}] parameter, [{}] given", parameterCount, count);

		} else {
			OtLogError("Function expects [{}] parameters, [{}] given", parameterCount, count);
		}
	}

	// execute function in VM
	auto result = OtVM::execute(bytecode, count);
	return result;
}


//
//	OtByteCodeFunctionClass::getMeta
//

OtType OtByteCodeFunctionClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtByteCodeFunctionClass>("ByteCodeFunction", OtInternalClass::getMeta());
		type->set("__call__", OtFunction::create(&OtByteCodeFunctionClass::operator()));
	}

	return type;
}
