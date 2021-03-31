//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "object.h"
#include "bytecode.h"
#include "global.h"
#include "stack.h"


//
//	The ObjectTalk Virtual Machine
//


class OtVM {
public:
	// initialize virtual machine
	static int init(int argc, char* argv[]);

	// terminate virtual machine
	static void end();

	// execute bytecode in the virtual machine
	static OtObject execute(OtByteCode bytecode, size_t callingParameters);

	// virtual machine components
	static OtGlobal global;
	static OtStack stack;
	static OtObject null;

private:
	static bool valgrindMode;
};
