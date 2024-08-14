//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtDebugger.h"
#include "OtException.h"
#include "OtFunction.h"
#include "OtVM.h"


//
//	OtDebuggerClass::debug
//

void OtDebuggerClass::debug(size_t count, OtObject* parameters) {
	if (count > 1) {
		OtError("Debug function requires 0 or 1 parameters, not {}", count);
	}

	if (count == 0 || parameters[0]->operator bool()) {

	}
}


//
//	OtDebuggerClass::getMeta
//

OtType OtDebuggerClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtGlobalClass>("Debugger", OtInternalClass::getMeta());
		type->set("__call__", OtFunction::create(&OtDebuggerClass::debug));
	}

	return type;
}
