//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>
#include <cstdlib>
#include <vector>

#include "fmt/format.h"
#include "isocline.h"

#include "OtArray.h"
#include "OtConfig.h"
#include "OtDebugger.h"
#include "OtException.h"
#include "OtFunction.h"
#include "OtString.h"
#include "OtText.h"
#include "OtVM.h"


//
//	OtDebuggerClass::debug
//

void OtDebuggerClass::debug(size_t count, OtObject* parameters) {
	if (count > 1) {
		OtError("Debug function requires 0 or 1 parameters, not {}", count);
	}

	if (count == 0 || parameters[0]->operator bool()) {
		// activate statement hook in VM
		OtVM::setStatementHook([this]() {
			if (OtVM::getStack()->getFrameCount() == stackFrame) {
				processCommand();
			}
		});

		// set our state
		stackFrame = OtVM::getStack()->getFrameCount();
	}
}


//
//	OtDebuggerClass::getVariableNames
//

OtObject OtDebuggerClass::getVariableNames() {
	// create list of variable names used at current location in current stack frame
	auto names = OtVM::getByteCode()->getUsedSymbolNames(OtVM::getPC());
	auto array = OtArray::create();

	for (auto& name : names) {
		array->append(OtString::create(name));
	}

	return array;
}


//
//	Completer support functions
//

static void wordCompleter(ic_completion_env_t* cenv, const char* word) {
	static const char* completions[] = {
		"help",
		"where",
		"disassemble",
		"continue",
		"step",
		"in",
		"out",
		"variables",
		"quit",
		nullptr};

	ic_add_completions(cenv, word, completions);
}

static void completer(ic_completion_env_t* cenv, const char* input) {
	ic_complete_word(cenv, input, &wordCompleter, nullptr);
}


//
//	Help text
//

static const char* help =
	"[ansi-blue]help[/]        show this help\n"
	"[ansi-blue]where[/]       show the current source line\n"
	"[ansi-blue]disassemble[/] disassemble the current function\n"
	"[ansi-blue]continue[/]    continue the program\n"
	"[ansi-blue]step[/]        step over the current instruction\n"
	"[ansi-blue]in[/]          step into the next function\n"
	"[ansi-blue]out[/]         step out of the current function\n"
	"[ansi-blue]variables[/]   show all relevant variables\n"
	"[ansi-blue]quit[/]        terminate the debugger\n";


//
//	OtDebuggerClass::processCommand
//

void OtDebuggerClass::processCommand() {
	// run the debugger
	bool debugging = true;

	while (debugging) {
		// initialize command line library (if required)
		static bool initialized = false;

		if (!initialized) {
			ic_set_default_completer(&completer, nullptr);
			ic_set_history(nullptr, -1);
			initialized = true;
		}

		// get command line input
		char* input = ic_readline("ot");

		// sanity check
		if (!input) {
			OtLogFatal("Internal error during ic_readline");
		}

		// parse command
		std::vector<std::string> command;
		OtText::split(OtText::compressWhitespace(input), command);

		if (command.size()) {
			// process each command
			if (command[0] == "help" || command[0] == "h") {
				ic_print(help);

			} else if (command[0] == "where" || command[0] == "w") {
				ic_print(where().c_str());

			} else if (command[0] == "disassemble" || command[0] == "d") {
				ic_print(disassemble().c_str());

			} else if (command[0] == "continue" || command[0] == "c") {
				OtVM::setStatementHook(nullptr);
				debugging = false;

			} else if (command[0] == "step" || command[0] == "s") {
				debugging = false;

			} else if (command[0] == "in" || command[0] == "i") {
				stackFrame++;
				debugging = false;

			} else if (command[0] == "out" || command[0] == "o") {
				stackFrame--;
				debugging = false;

			} else if (command[0] == "variables" || command[0] == "v") {
				ic_print(variables().c_str());

			} else if (command[0] == "quit" || command[0] == "q") {
				std::_Exit(EXIT_FAILURE);

			} else {
				ic_println(fmt::format("[red]Unknown command: {}[/red]", input).c_str());
			}

			ic_free(input);
		}
	}
}


//
//	OtDebuggerClass::where
//

std::string OtDebuggerClass::where() {
	auto bytecode = OtVM::getByteCode();

	return fmt::format(
		"Module: {}\n{}\n",
		bytecode->getModule(),
		bytecode->getStatementSourceCode(OtVM::getPC()));
}


//
//	OtDebuggerClass::disassemble
//

std::string OtDebuggerClass::disassemble() {
	return OtVM::getByteCode()->disassemble();
}


//
//	OtDebuggerClass::variables
//

std::string OtDebuggerClass::variables() {
	// get list of variable used in current context
	auto symbols = OtVM::getByteCode()->getUsedSymbols(OtVM::getPC());

	// determine names and values
	std::vector<std::pair<std::string, OtObject>> variables;

	for (auto& symbol : symbols) {
		auto name = std::string(OtIdentifier::name(symbol.id));
		OtObject value;

		switch (symbol.type) {
			case OtSymbol::Type::heap:
				value = symbol.object->get(symbol.id);
				break;

			case OtSymbol::Type::stack:
				value = OtVM::getStack()->getFrameItem(symbol.slot);
				break;

			case OtSymbol::Type::capture:
				break;
		}

		variables.emplace_back(name, value);
	}

	// determine longest strings
	size_t longestName = 8;
	size_t longestClassName = 5;

	for (auto& variable : variables) {
		longestName = std::max(longestName, variable.first.size());
	}

	for (auto& variable : variables) {
		longestClassName = std::max(longestClassName, variable.second->getType()->getName().size());
	}

	// format output
	std::string result = OtText::pad("Variable", longestName + 2) + OtText::pad("Class", longestClassName + 2) + "Details\n";

	for (auto& variable : variables) {
		auto variableName = OtText::pad(variable.first, longestName + 2);
		auto classType = variable.second->getType()->getName();
		auto className = OtText::pad(classType, longestClassName + 2);
		auto description = variable.second->describe();

		if (description == classType) {
			description = " ";
		}

		result += variableName + className + description + "\n";
	}

	return result;
}


//
//	OtDebuggerClass::getMeta
//

OtType OtDebuggerClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtGlobalClass>("Debugger", OtInternalClass::getMeta());
		type->set("__call__", OtFunction::create(&OtDebuggerClass::debug));
		type->set("where", OtFunction::create(&OtDebuggerClass::where));
		type->set("disassemble", OtFunction::create(&OtDebuggerClass::disassemble));
		type->set("getVariableNames", OtFunction::create(&OtDebuggerClass::getVariableNames));
	}

	return type;
}
