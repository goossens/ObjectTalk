//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>

#include "OtByteCode.h"
#include "OtInternal.h"


//
//	OtDebugger
//

class OtDebuggerClass;
using OtDebugger = OtObjectPointer<OtDebuggerClass>;

class OtDebuggerClass : public OtInternalClass {
public:
	// enter debugger
	void debug(size_t count, OtObject* parameters);

	// get current location information
	std::string where();

	// get disassembled bytecode for current function
	std::string disassemble();

	// get type definition
	static OtType getMeta();

private:
	// process a debug command
	void processCommand();

	// debugger state
	bool breakOnInstruction = false;
	size_t stackFrame = 0;
	size_t instructionStart;

	OtByteCode bytecode;
	size_t pc;
};
