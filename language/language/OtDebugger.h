//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>

#include "OtByteCode.h"
#include "OtDebugState.h"
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

	// get variable names
	OtObject getVariableNames();

	// get type definition
	static OtType getMeta();

protected:
	// constructor
	friend class OtObjectPointer<OtDebuggerClass>;
	OtDebuggerClass() = default;

private:
	// run the debug hook
	void debugHook();

	// run debugger over stdio back to IDE
	void processIDE();

	// run debugger from command line
	void processCommandLine();

	// flow control
	bool continueCommand();
	bool stepOverCommand();
	bool stepInCommand();
	bool stepOutCommand();
	bool isBreaking();

	// get current location information
	std::string where();

	// get disassembled bytecode for current function
	std::string disassemble();

	// get current variables and values
	std::string getVariables();

	// debugger state
	enum class State {
		running,
		stopped,
		stepOver,
		stepIn,
		stepOut
	} state = State::running;

	size_t targetStackFrame;
};
