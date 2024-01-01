//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <fstream>
#include <sstream>
#include <vector>

#include "OtByteCodeFunction.h"
#include "OtCaptureReference.h"
#include "OtClass.h"
#include "OtClosure.h"
#include "OtCompiler.h"
#include "OtException.h"
#include "OtInteger.h"
#include "OtMemberReference.h"
#include "OtReal.h"
#include "OtSelector.h"
#include "OtStackReference.h"
#include "OtString.h"
#include "OtThrow.h"


OtByteCode OtCompiler::compileFile(const std::filesystem::path& path, OtObject object, bool disassemble) {
	// sanity check
	if (!std::filesystem::exists(path)) {
		OtError("Can't open file [%s]", path.string().c_str());
	}

	// load source code and compile into bytecode
	std::ifstream stream(path);
	std::stringstream buffer;
	buffer << stream.rdbuf();
	stream.close();
	OtSource source = OtSourceClass::create(path.string(), buffer.str());
	return compileSource(source, object, disassemble);
}


//
//	OtCompiler::compileText
//

OtByteCode OtCompiler::compileText(const std::string& text, OtObject object, bool disassemble) {
	// convert to source object, compile and return bytecode
	OtSource source = OtSourceClass::create("__internal__", text);
	return compileSource(source, object, disassemble);
}


//
//	OtCompiler::compileSource
//

OtByteCode OtCompiler::compileSource(OtSource src, OtObject object, bool disassemble) {
	// remember source code and flag
	source = src;
	disassembleBytecode = disassemble;

	// clear scope stack
	scopeStack.clear();

	// load scanner
	scanner.loadSource(src);

	// setup bytecode
	OtByteCode bytecode = OtByteCode::create(src);

	// setup global scope
	std::vector<std::string_view> names;
	OtGlobal global = OtVM::instance()->getGlobal();
	global->getMemberNames(names);
	pushObjectScope(global);

	for (auto name : names) {
		declareVariable(bytecode, std::string(name));
	}

	// setup object scope
	pushObjectScope(object);
	object->getMemberNames(names);

	for (auto name : names) {
		declareVariable(bytecode, std::string(name));
	}

	// process all statements
	while (!scanner.matchToken(OtScanner::EOS_TOKEN)) {
		statement(bytecode);
	}

	// clear scope stack
	scopeStack.clear();

	// ensure we leave a default result on the stack
	bytecode->push(OtVM::instance()->getNull());

	// optimize code
	auto optimized = optimizer.optimize(bytecode);

	// disassemble raw and optimized code
	if (disassemble) {
		outputDisassembledBytecode(bytecode, optimized);
	}

	// return compiled and optimized code
	return optimized;
}


//
//	OtCompiler::compileExpression
//

OtByteCode OtCompiler::compileExpression(OtSource src, bool disassemble) {
	// rememember source code
	source = src;

	// load scanner
	scanner.loadSource(src);

	// clear scope stack
	scopeStack.clear();

	// setup bytecode
	OtByteCode bytecode = OtByteCode::create(src);

	// setup global scope
	std::vector<std::string_view> names;
	OtGlobal global = OtVM::instance()->getGlobal();
	global->getMemberNames(names);
	pushObjectScope(global);

	for (auto name : names) {
		declareVariable(bytecode, std::string(name));
	}

	// process expression
	if (expression(bytecode)) {
		bytecode->method("__deref__", 0);
	}

	scanner.expect(OtScanner::EOS_TOKEN);

	// clear scope stack
	scopeStack.clear();

	// optimize code
	auto optimized = optimizer.optimize(bytecode);

	// disassemble raw and optimized code
	if (disassemble) {
		outputDisassembledBytecode(bytecode, optimized);
	}

	// return compiled and optimized code
	return optimized;
}


//
//	OtCompiler::pushObjectScope
//

void OtCompiler::pushObjectScope(OtObject object) {
	scopeStack.emplace_back(OtScope(OBJECT_SCOPE, object));
}


//
//	OtCompiler::pushFunctionScope
//

void OtCompiler::pushFunctionScope() {
	scopeStack.emplace_back(OtScope(FUNCTION_SCOPE));
}


//
//	OtCompiler::pushBlockScope
//

void OtCompiler::pushBlockScope() {
	auto scope = &(scopeStack.back());
	auto stackFrameOffset = scope->stackFrameOffset;

	if (scope->type == FUNCTION_SCOPE || scope->type == BLOCK_SCOPE) {
		stackFrameOffset += scope->locals.size();
	}

	scopeStack.emplace_back(OtScope(BLOCK_SCOPE, stackFrameOffset));
}


//
//	OtCompiler::popScope
//

void OtCompiler::popScope() {
	// remove last scope
	scopeStack.pop_back();
}


//
//	OtCompiler::declareCapture
//

void OtCompiler::declareCapture(const std::string& name, OtStackItem item) {
	// find most recent function scope
	auto scope = scopeStack.rbegin();

	while (scope->type != FUNCTION_SCOPE && scope != scopeStack.rend()) {
		scope++;
	}

	// sanity check
	if (scope == scopeStack.rend()) {
		OtError("Internal error. No function scope on stack");
	}

	// see if this variable is already captured
	if (scope->captures.count(name)) {
		if (scope->captures[name] != item) {
			OtError("Internal error. Captured variable has different stack offset");
		}

	} else {
		// add item to list of captured variables
		scope->captures[name] = item;
	}
}


//
//	OtCompiler::declareVariable
//

void OtCompiler::declareVariable(OtByteCode bytecode, const std::string& name, bool alreadyOnStack) {
	// get current scope
	auto scope = &(scopeStack.back());

	// avoid double declaration
	if (scope->locals.count(name)) {
		scanner.error(OtFormat("Variable [%s] already defined in this scope", name.c_str()));
	}

	// add variable to compiler scope
	if (scope->type == FUNCTION_SCOPE || scope->type == BLOCK_SCOPE) {
		// variable lives on the stack
		scope->locals[name] = scope->stackFrameOffset + scope->locals.size();

		// reserve space
		if (!alreadyOnStack) {
			bytecode->reserve();
		}

	} else {
		// variable lives on the heap
		scope->locals[name] = 0;
	}
}


//
//	OtCompiler::resolveVariable
//

void OtCompiler::resolveVariable(OtByteCode bytecode, const std::string& name) {
	// try to resolve name
	bool found = false;
	size_t functionLevel = 0;

	// look at all scope levels in reverse order
	for (auto scope = scopeStack.rbegin(); !found && scope != scopeStack.rend(); scope++) {
		// is variable in this scope?
		if (scope->locals.count(name)) {
			// yes, handle different scope types
			switch(scope->type) {
				case OBJECT_SCOPE:
					// variable is object member
					bytecode->push(OtMemberReference::create(scope->object, OtSelector::create(name)));
					break;

				case FUNCTION_SCOPE:
				case BLOCK_SCOPE:
					if (functionLevel == 0) {
						// variable lives on the stack in the current stack frame
						bytecode->push(OtStackReference::create(name, scope->locals[name]));

					} else {
						// variable is in an enclosing function, we need to capture it
						declareCapture(name, OtStackItem(functionLevel - 1, scope->locals[name]));
						bytecode->push(OtCaptureReference::create(OtSelector::create(name)));
					}

					break;

				default:
					OtError("Internal Error: Invalid scope type");
			}

			found = true;
		}

		// do some housekeeping if we had a function scope
		if (!found && scope->type == FUNCTION_SCOPE) {
			// track how many function levels we go back
			functionLevel++;
		}
	}

	// generate error if variable is not found
	if (!found) {
		scanner.error(OtFormat("Unknown variable [%s]", name.c_str()));
	}
}


//
//	OtCompiler::assignVariable
//

void OtCompiler::assignVariable(OtByteCode bytecode, const std::string& name) {
	resolveVariable(bytecode, name);
	bytecode->swap();
	bytecode->method("__assign__", 1);
	bytecode->pop();
}


//
//	OtCompiler::function
//

void OtCompiler::function(OtByteCode bytecode) {
	// start a new function scope
	pushFunctionScope();

	// parse calling parameters
	scanner.expect(OtScanner::LPAREN_TOKEN);
	size_t count = 0;

	while (!scanner.matchToken(OtScanner::RPAREN_TOKEN) && !scanner.matchToken(OtScanner::EOS_TOKEN)) {
		scanner.expect(OtScanner::IDENTIFIER_TOKEN, false);
		declareVariable(bytecode, scanner.getText(), true);
		scanner.advance();
		count++;

		if (!scanner.matchToken(OtScanner::RPAREN_TOKEN)) {
			scanner.expect(OtScanner::COMMA_TOKEN);
		}
	}

	scanner.expect(OtScanner::RPAREN_TOKEN);

	// each function has its own bytecode
	OtByteCode functionCode = OtByteCode::create(source);

	// get function level bytecode
	block(functionCode);

	// default return value in case function does not have return statement
	functionCode->push(OtVM::instance()->getNull());

	// optimize code
	auto optimized = optimizer.optimize(functionCode);

	// disassemble raw and optimized code
	if (disassembleBytecode) {
		outputDisassembledBytecode(functionCode, optimized);
	}

	// create a new bytecode function
	auto function = OtByteCodeFunction::create(optimized, count);

	// see if this function captures variables and needs a closure?
	auto scope = &(scopeStack.back());

	if (scope->captures.size()) {
		// function does capture variables so let's wrap it in a closure
		bytecode->push(OtClosure::create(function, scope->captures));

		// generate code to perform the actual capture
		bytecode->method("__capture__", 0);

	} else {
		// no captures so just put new function on the stack
		bytecode->push(function);
	}

	// end function scope
	popScope();
}


//
//	OtCompiler::primary
//

bool OtCompiler::primary(OtByteCode bytecode) {
	bool reference;
	size_t count = 0;

	switch (scanner.getToken()) {
		case OtScanner::LPAREN_TOKEN:
			// handle sub-expression
			scanner.advance();
			reference = expression(bytecode);
			scanner.expect(OtScanner::RPAREN_TOKEN);
			break;

		case OtScanner::INTEGER_TOKEN:
			// handle integer constants
			bytecode->push(OtInteger::create(scanner.getInteger()));
			scanner.advance();
			reference = false;
			break;

		case OtScanner::REAL_TOKEN:
			// handle real constants
			bytecode->push(OtReal::create(scanner.getReal()));
			scanner.advance();
			reference = false;
			break;

		case OtScanner::STRING_TOKEN:
			// handle string constants
			bytecode->push(OtString::create(scanner.getString()));
			scanner.advance();
			reference = false;
			break;

		case OtScanner::FUNCTION_TOKEN:
			// handle function definition
			scanner.advance();
			function(bytecode);
			reference = false;
			break;

		case OtScanner::IDENTIFIER_TOKEN: {
			// handle named reference
			auto name = scanner.getText();
			resolveVariable(bytecode, name);
			scanner.advance();
			reference = true;
			break;
		}

		case OtScanner::LBRACKET_TOKEN:
			// handle array constant
			scanner.advance();
			resolveVariable(bytecode, "Array");
			bytecode->method("__deref__", 0);
			bytecode->method("__call__", 0);
			bytecode->dup();

			if (scanner.getToken() != OtScanner::RBRACKET_TOKEN) {
				bytecode->method("__init__", expressions(bytecode));
			}

			bytecode->pop();
			scanner.expect(OtScanner::RBRACKET_TOKEN);
			reference = false;
			break;

		case OtScanner::LBRACE_TOKEN:
			// handle dictionary constant
			scanner.advance();
			resolveVariable(bytecode, "Dict");
			bytecode->method("__deref__", 0);
			bytecode->method("__call__", 0);
			bytecode->dup();

			while (scanner.getToken() != OtScanner::RBRACE_TOKEN && scanner.getToken() != OtScanner::EOS_TOKEN) {
				scanner.expect(OtScanner::STRING_TOKEN, false);
				bytecode->push(OtString::create(scanner.getString()));
				scanner.advance();
				scanner.expect(OtScanner::COLON_TOKEN);

				if (expression(bytecode)) {
					bytecode->method("__deref__", 0);
				}

				count += 2;

				if (scanner.getToken() == OtScanner::COMMA_TOKEN) {
					scanner.advance();
				}
			}

			scanner.expect(OtScanner::RBRACE_TOKEN);

			if (count) {
				bytecode->method("__init__", count);
			}

			bytecode->pop();
			reference = false;
			break;

		default:
			reference = false;
			scanner.error("Primary expected");
	}

	return reference;
}


//
//	OtCompiler::postfix
//

bool OtCompiler::postfix(OtByteCode bytecode) {
	// parse left side
	bool reference = primary(bytecode);
	OtToken token = scanner.getToken();

	// process postfix
	while (token == OtScanner::LBRACKET_TOKEN ||
		   token == OtScanner::LPAREN_TOKEN ||
		   token == OtScanner::PERIOD_TOKEN ||
		   token == OtScanner::INCREMENT_TOKEN ||
		   token == OtScanner::DECREMENT_TOKEN) {
		scanner.advance();
		std::string member;
		size_t count;

		// generate code
		switch (token) {
			case OtScanner::LBRACKET_TOKEN:
				// index object
				if (reference) {
					bytecode->method("__deref__", 0);
				}

				if (expression(bytecode)) {
					bytecode->method("__deref__", 0);
				}

				scanner.expect(OtScanner::RBRACKET_TOKEN);
				bytecode->method("__index__", 1);
				reference = true;
				break;

			case OtScanner::LPAREN_TOKEN:
				// call object
				if (reference) {
					bytecode->method("__deref__", 0);
				}

				count = scanner.matchToken(OtScanner::RPAREN_TOKEN) ? 0 : expressions(bytecode);
				scanner.expect(OtScanner::RPAREN_TOKEN);
				bytecode->method("__call__", count);
				reference = false;
				break;

			case OtScanner::PERIOD_TOKEN:
				// member access
				if (reference) {
					bytecode->method("__deref__", 0);
				}

				scanner.expect(OtScanner::IDENTIFIER_TOKEN, false);
				bytecode->member(scanner.getText());
				scanner.advance();
				reference = true;

				break;

			case OtScanner::INCREMENT_TOKEN:
				if (!reference) {
					scanner.error("Lvalue required for '++'");
				}

				bytecode->dup();
				bytecode->method("__deref__", 0);
				bytecode->swap();
				bytecode->dup();
				bytecode->method("__deref__", 0);
				bytecode->method("__inc__", 0);
				bytecode->method("__assign__", 1);
				bytecode->pop();
				reference = false;
				break;

			case OtScanner::DECREMENT_TOKEN:
				if (!reference) {
					scanner.error("Lvalue required for '--'");
				}

				bytecode->dup();
				bytecode->method("__deref__", 0);
				bytecode->swap();
				bytecode->dup();
				bytecode->method("__deref__", 0);
				bytecode->method("__dec__", 0);
				bytecode->method("__assign__", 1);
				bytecode->pop();
				reference = false;
				break;
		}

		token = scanner.getToken();
	}

	return reference;
}


//
//	OtCompiler::prefix
//

bool OtCompiler::prefix(OtByteCode bytecode) {
	OtToken token = scanner.getToken();

	// process prefix
	if (token == OtScanner::SUBTRACT_TOKEN ||
		token == OtScanner::NEGATE_TOKEN ||
		token == OtScanner::BITWISE_NOT_TOKEN ||
		token == OtScanner::INCREMENT_TOKEN ||
		token == OtScanner::DECREMENT_TOKEN) {
		scanner.advance();
		bool reference = postfix(bytecode);

		// generate code
		switch (token) {
			case OtScanner::SUBTRACT_TOKEN:
				if (reference) {
					bytecode->method("__deref__", 0);
				}

				bytecode->method("__neg__", 0);
				break;

		case OtScanner::ADD_TOKEN:
			if (reference) {
				bytecode->method("__deref__", 0);
			}

			bytecode->method("__plus__", 0);
			break;

			case OtScanner::NEGATE_TOKEN:
				if (reference) {
					bytecode->method("__deref__", 0);
				}

				bytecode->method("__not__", 0);
				break;

			case OtScanner::BITWISE_NOT_TOKEN:
				if (reference) {
					bytecode->method("__deref__", 0);
				}

				bytecode->method("__bnot__", 0);
				break;

			case OtScanner::INCREMENT_TOKEN:
				if (!reference) {
					scanner.error("Lvalue required for '++'");
				}

				bytecode->dup();
				bytecode->method("__deref__", 0);
				bytecode->method("__inc__", 0);
				bytecode->method("__assign__", 1);
				break;

			case OtScanner::DECREMENT_TOKEN:
				if (!reference) {
					scanner.error("Lvalue required for '--'");
				}

				bytecode->dup();
				bytecode->method("__deref__", 0);
				bytecode->method("__dec__", 0);
				bytecode->method("__assign__", 1);
				break;
		}

		return false;

	} else {
		return postfix(bytecode);
	}
}


//
//	OtCompiler::multiplications
//

bool OtCompiler::multiplications(OtByteCode bytecode) {
	// parse left side
	bool reference = prefix(bytecode);
	OtToken token = scanner.getToken();

	while (token == OtScanner::MULTIPLY_TOKEN ||
		   token == OtScanner::DIVIDE_TOKEN ||
		   token == OtScanner::POWER_TOKEN ||
		   token == OtScanner::MODULO_TOKEN) {
		// deref if required
		scanner.advance();

		if (reference) {
			bytecode->method("__deref__", 0);
		}

		// parse right side
		if (prefix(bytecode)) {
			bytecode->method("__deref__", 0);
		}

		// generate code
		switch (token) {
			case OtScanner::MULTIPLY_TOKEN:
				bytecode->method("__mul__", 1);
				break;

			case OtScanner::DIVIDE_TOKEN:
				bytecode->method("__div__", 1);
				break;

			case OtScanner::POWER_TOKEN:
				bytecode->method("__power__", 1);
				break;

			case OtScanner::MODULO_TOKEN:
				bytecode->method("__mod__", 1);
				break;
		}

		token = scanner.getToken();
		reference = false;
	}

	return reference;
}


//
//	OtCompiler::additions
//

bool OtCompiler::additions(OtByteCode bytecode) {
	// parse left side
	bool reference = multiplications(bytecode);
	OtToken token = scanner.getToken();

	while (token == OtScanner::ADD_TOKEN || token == OtScanner::SUBTRACT_TOKEN) {
		// deref if required
		scanner.advance();

		if (reference) {
			bytecode->method("__deref__", 0);
		}

		// parse right side
		if (multiplications(bytecode)) {
			bytecode->method("__deref__", 0);
		}

		// generate code
		if (token == OtScanner::ADD_TOKEN) {
			bytecode->method("__add__", 1);

		} else {
			bytecode->method("__sub__", 1);
		}

		token = scanner.getToken();
		reference = false;
	}

	return reference;
}


//
//	OtCompiler::shift
//

bool OtCompiler::shift(OtByteCode bytecode) {
	// parse left side
	bool reference = additions(bytecode);
	OtToken token = scanner.getToken();

	while (token == OtScanner::SHIFT_LEFT_TOKEN || token == OtScanner::SHIFT_RIGHT_TOKEN) {
		// deref if required
		scanner.advance();

		if (reference) {
			bytecode->method("__deref__", 0);
		}

		// parse right side
		if (additions(bytecode)) {
			bytecode->method("__deref__", 0);
		}

		// generate code
		if (token == OtScanner::SHIFT_LEFT_TOKEN) {
			bytecode->method("__lshift__", 1);

		} else {
			bytecode->method("__rshift__", 1);
		}

		token = scanner.getToken();
		reference = false;
	}

	return reference;
}


//
//	OtCompiler::relation
//

bool OtCompiler::relation(OtByteCode bytecode) {
	// parse left side
	bool reference = shift(bytecode);
	OtToken token = scanner.getToken();

	while (token == OtScanner::LESS_TOKEN ||
		   token == OtScanner::LESS_EQUAL_TOKEN ||
		   token == OtScanner::GREATER_TOKEN ||
		   token == OtScanner::GREATER_EQUAL_TOKEN ||
		   token == OtScanner::IN_TOKEN ||
		   token == OtScanner::NOT_TOKEN) {
		scanner.advance();

		// expect "in" if required
		if (token == OtScanner::NOT_TOKEN) {
			scanner.expect(OtScanner::IN_TOKEN);
		}

		// deref if required
		if (reference) {
			bytecode->method("__deref__", 0);
		}

		// parse right side
		if (shift(bytecode)) {
			bytecode->method("__deref__", 0);
		}

		// generate code
		switch (token) {
			case OtScanner::LESS_TOKEN:
				bytecode->method("__lt__", 1);
				break;

			case OtScanner::LESS_EQUAL_TOKEN:
				bytecode->method("__le__", 1);
				break;

			case OtScanner::GREATER_TOKEN:
				bytecode->method("__gt__", 1);
				break;

			case OtScanner::GREATER_EQUAL_TOKEN:
				bytecode->method("__ge__", 1);
				break;

			case OtScanner::IN_TOKEN:
				bytecode->swap();
				bytecode->method("__contains__", 1);
				break;

			case OtScanner::NOT_TOKEN:
				bytecode->swap();
				bytecode->method("__contains__", 1);
				bytecode->method("__not__", 0);
				break;
		}

		token = scanner.getToken();
		reference = false;
	}

	return reference;
}


//
//	OtCompiler::equal
//

bool OtCompiler::equal(OtByteCode bytecode) {
	// parse left side
	bool reference = relation(bytecode);
	OtToken token = scanner.getToken();

	while (token == OtScanner::EQUAL_TOKEN || token == OtScanner::NOT_EQUAL_TOKEN) {
		scanner.advance();

		// deref if required
		if (reference) {
			bytecode->method("__deref__", 0);
		}

		// parse right side
		if (relation(bytecode)) {
			bytecode->method("__deref__", 0);
		}

		// generate code
		if (token == OtScanner::EQUAL_TOKEN) {
			bytecode->method("__eq__", 1);
		} else {
			bytecode->method("__ne__", 1);
		}

		token = scanner.getToken();
		reference = false;
	}

	return reference;
}


//
//	OtCompiler::bitwiseAnd
//

bool OtCompiler::bitwiseAnd(OtByteCode bytecode) {
	// parse left side
	bool reference = equal(bytecode);

	// handle all "&" operators
	if (scanner.matchToken(OtScanner::BITWISE_AND_TOKEN)) {
		// deref if required
		scanner.advance();

		if (reference) {
			bytecode->method("__deref__", 0);
		}

		// parse right side
		if (equal(bytecode)) {
			bytecode->method("__deref__", 0);
		}

		// generate code
		bytecode->method("__band__", 1);
		reference = false;
	}

	return reference;
}


//
//	OtCompiler::bitwiseXor
//

bool OtCompiler::bitwiseXor(OtByteCode bytecode) {
	// parse left side
	bool reference = bitwiseAnd(bytecode);

	// handle all "&" operators
	if (scanner.matchToken(OtScanner::BITWISE_XOR_TOKEN)) {
		// deref if required
		scanner.advance();

		if (reference) {
			bytecode->method("__deref__", 0);
		}

		// parse right side
		if (bitwiseAnd(bytecode)) {
			bytecode->method("__deref__", 0);
		}

		// generate code
		bytecode->method("__bxor__", 1);
		reference = false;
	}

	return reference;
}


//
//	OtCompiler::bitwiseOr
//

bool OtCompiler::bitwiseOr(OtByteCode bytecode) {
	// parse left side
	bool reference = bitwiseXor(bytecode);

	// handle all "|" operators
	if (scanner.matchToken(OtScanner::BITWISE_OR_TOKEN)) {
		// deref if required
		scanner.advance();

		if (reference) {
			bytecode->method("__deref__", 0);
		}

		// parse right side
		if (bitwiseXor(bytecode)) {
			bytecode->method("__deref__", 0);
		}

		// generate code
		bytecode->method("__bor__", 1);
		reference = false;
	}

	return reference;
}


//
//	OtCompiler::andExpression
//

bool OtCompiler::andExpression(OtByteCode bytecode) {
	// parse left side
	bool reference = bitwiseOr(bytecode);

	// handle all "&&" operators
	while (scanner.matchToken(OtScanner::AND_TOKEN)) {
		// skip token
		scanner.advance();

		// deref if required
		if (reference) {
			bytecode->method("__deref__", 0);
		}

		// parse right side
		if (bitwiseOr(bytecode)) {
			bytecode->method("__deref__", 0);
		}

		// generate code
		bytecode->method("__and__", 1);
		reference = false;
	}

	return reference;
}


//
//	OtCompiler::orExpression
//

bool OtCompiler::orExpression(OtByteCode bytecode) {
	// parse left side
	bool reference = andExpression(bytecode);

	// handle all "||" operators
	while (scanner.matchToken(OtScanner::OR_TOKEN)) {
		// skip token
		scanner.advance();

		// deref if required
		if (reference) {
			bytecode->method("__deref__", 0);
		}

		// parse right side
		if (andExpression(bytecode)) {
			bytecode->method("__deref__", 0);
		}

		// generate code
		bytecode->method("__or__", 1);
		reference = false;
	}

	return reference;
}


//
//	OtCompiler::conditional
//

bool OtCompiler::conditional(OtByteCode bytecode) {
	// parse left side
	bool reference = orExpression(bytecode);

	if (scanner.matchToken(OtScanner::QUESTION_TOKEN)) {
		// deref if required
		scanner.advance();

		if (reference) {
			bytecode->method("__deref__", 0);
		}

		// evaluate expression
		size_t offset1 = bytecode->jumpFalse(0);

		// process "true" expression
		if (expression(bytecode)) {
			bytecode->method("__deref__", 0);
		}

		scanner.expect(OtScanner::COLON_TOKEN);

		// jump around "false" expression and patch first jump
		size_t offset2 = bytecode->jump(0);
		bytecode->patchJump(offset1);

		// process "false" expression
		if (expression(bytecode)) {
			bytecode->method("__deref__", 0);
		}

		// patch jump around false
		bytecode->patchJump(offset2);
		reference = false;
	}

	return reference;
}


//
//	OtCompiler::expression
//

bool OtCompiler::expression(OtByteCode bytecode) {
	// parse left side
	bool reference = conditional(bytecode);
	OtToken token = scanner.getToken();

	// process all assignment operators
	while (token == OtScanner::ASSIGNMENT_TOKEN ||
		   token == OtScanner::MULTIPLY_ASSIGNMENT_TOKEN ||
		   token == OtScanner::DIVIDE_ASSIGNMENT_TOKEN ||
		   token == OtScanner::MODULO_ASSIGNMENT_TOKEN ||
		   token == OtScanner::ADD_ASSIGNMENT_TOKEN ||
		   token == OtScanner::SUBTRACT_ASSIGNMENT_TOKEN ||
		   token == OtScanner::SHIFT_LEFT_ASSIGNMENT_TOKEN ||
		   token == OtScanner::SHIFT_RIGHT_ASSIGNMENT_TOKEN ||
		   token == OtScanner::BITWISE_AND_ASSIGNMENT_TOKEN ||
		   token == OtScanner::BITWISE_OR_ASSIGNMENT_TOKEN ||
		   token == OtScanner::BITWISE_XOR_ASSIGNMENT_TOKEN) {
		// sanity check
		if (!reference) {
			scanner.error("Lvalue required for assignments");
		}

		// duplicate left side if required
		if (token != OtScanner::ASSIGNMENT_TOKEN) {
			bytecode->dup();
			bytecode->method("__deref__", 0);
		}

		// parse right side
		scanner.advance();

		if (expression(bytecode)) {
			bytecode->method("__deref__", 0);
		}

		// handle different assignments
		switch (token) {
			case OtScanner::MULTIPLY_ASSIGNMENT_TOKEN:
				bytecode->method("__mul__", 1);
				break;

			case OtScanner::DIVIDE_ASSIGNMENT_TOKEN:
				bytecode->method("__div__", 1);
				break;

			case OtScanner::MODULO_ASSIGNMENT_TOKEN:
				bytecode->method("__mod__", 1);
				break;

			case OtScanner::ADD_ASSIGNMENT_TOKEN:
				bytecode->method("__add__", 1);
				break;

			case OtScanner::SUBTRACT_ASSIGNMENT_TOKEN:
				bytecode->method("__sub__", 1);
				break;

			case OtScanner::SHIFT_LEFT_ASSIGNMENT_TOKEN:
				bytecode->method("__lshift__", 1);
				break;

			case OtScanner::SHIFT_RIGHT_ASSIGNMENT_TOKEN:
				bytecode->method("__rshift__", 1);
				break;

			case OtScanner::BITWISE_AND_ASSIGNMENT_TOKEN:
				bytecode->method("__band__", 1);
				break;

			case OtScanner::BITWISE_OR_ASSIGNMENT_TOKEN:
				bytecode->method("__bor__", 1);
				break;

			case OtScanner::BITWISE_XOR_ASSIGNMENT_TOKEN:
				bytecode->method("__bxor__", 1);
				break;
		}

		// perform assignment
		bytecode->method("__assign__", 1);
		reference = false;

		token = scanner.getToken();
	}

	return reference;
}


//
//	OtCompiler::expressions
//

size_t OtCompiler::expressions(OtByteCode bytecode) {
	// process comma seperated list of expressions
	size_t count = 1;

	if (expression(bytecode)) {
		bytecode->method("__deref__", 0);
	}

	while (scanner.matchToken(OtScanner::COMMA_TOKEN)) {
		scanner.advance();

		if (expression(bytecode)) {
			bytecode->method("__deref__", 0);
		}

		count++;
	}

	return count;
}


//
//	OtCompiler::block
//

void OtCompiler::block(OtByteCode bytecode) {
	scanner.expect(OtScanner::LBRACE_TOKEN);

	// enter a new block scope
	pushBlockScope();

	// process all statements in block
	while (!scanner.matchToken(OtScanner::RBRACE_TOKEN) && !scanner.matchToken(OtScanner::EOS_TOKEN)) {
		statement(bytecode);
	}

	// we better have the end of a block
	scanner.expect(OtScanner::RBRACE_TOKEN);

	// get number of locals on stack
	auto locals = scopeStack.back().locals.size();

	// locals are going out of scope so we remove them from the stack
	if (locals) {
		bytecode->pop(locals);
	}

	// remove the block scope
	popScope();
}


//
//	OtCompiler::variableDeclaration
//

void OtCompiler::variableDeclaration(OtByteCode bytecode) {
	// handle variable name
	scanner.expect(OtScanner::VAR_TOKEN);
	scanner.expect(OtScanner::IDENTIFIER_TOKEN, false);
	std::string name = scanner.getText();

	// add variable to scope
	declareVariable(bytecode, name);
	scanner.advance();

	// process initial value if required
	if (scanner.matchToken(OtScanner::ASSIGNMENT_TOKEN)) {
		// process expression
		scanner.advance();

		if (expression(bytecode)) {
			bytecode->method("__deref__", 0);
		}

		assignVariable(bytecode, name);
	}

	// must have semicolon at the end of a variable declaration
	scanner.expect(OtScanner::SEMICOLON_TOKEN);
}


//
//	OtCompiler::classDeclaration
//

void OtCompiler::classDeclaration(OtByteCode bytecode) {
	// handle class name
	scanner.expect(OtScanner::CLASS_TOKEN);
	scanner.expect(OtScanner::IDENTIFIER_TOKEN, false);
	std::string name = scanner.getText();

	// create new class
	OtClass cls = OtClass::create(name);

	// add class to current scope
	declareVariable(bytecode, name);
	bytecode->push(cls);
	assignVariable(bytecode, name);
	scanner.advance();

	// handle parent class
	scanner.expect(OtScanner::COLON_TOKEN);
	bytecode->push(cls);

	if (expression(bytecode)) {
		bytecode->method("__deref__", 0);
	}

	bytecode->method("setParent", 1);
	bytecode->pop();

	// start new class scope
	pushObjectScope(cls);

	// process class content
	scanner.expect(OtScanner::LBRACE_TOKEN);

	while (!scanner.matchToken(OtScanner::RBRACE_TOKEN) && !scanner.matchToken(OtScanner::EOS_TOKEN)) {
		statement(bytecode);
	}

	scanner.expect(OtScanner::RBRACE_TOKEN);

	// end class scope
	popScope();
}


//
//	OtCompiler::functionDeclaration
//

void OtCompiler::functionDeclaration(OtByteCode bytecode) {
	// get function name
	scanner.expect(OtScanner::FUNCTION_TOKEN);
	scanner.expect(OtScanner::IDENTIFIER_TOKEN, false);
	std::string name = scanner.getText();

	// add function to scope to allow for recursion
	declareVariable(bytecode, name);
	scanner.advance();

	// parse function definition to function object on stack
	function(bytecode);

	// assign function object to variable name
	assignVariable(bytecode, name);
}


//
//	OtCompiler::doStatement
//

void OtCompiler::doStatement(OtByteCode bytecode) {
	scanner.expect(OtScanner::DO_TOKEN);
	size_t offset = bytecode->size();

	block(bytecode);

	scanner.expect(OtScanner::WHILE_TOKEN);

	if (expression(bytecode)) {
		bytecode->method("__deref__", 0);
	}

	bytecode->jumpTrue(offset);
	scanner.expect(OtScanner::SEMICOLON_TOKEN);
}


//
//	OtCompiler::forStatement
//

void OtCompiler::forStatement(OtByteCode bytecode) {
	scanner.expect(OtScanner::FOR_TOKEN);

	// "for loop" is "fake" block to handle loop variables
	pushBlockScope();

	// get name of iteration variable
	scanner.expect(OtScanner::IDENTIFIER_TOKEN, false);
	std::string name = scanner.getText();

	// create loop variable on the stack
	declareVariable(bytecode, name);

	// get the object to be iterated on
	scanner.advance();
	scanner.expect(OtScanner::IN_TOKEN);

	if (expression(bytecode)) {
		bytecode->method("__deref__", 0);
	}

	// pretend iterator is a local variable (that can't be addressed)
	declareVariable(bytecode, "", true);

	// turn object into an iterator
	bytecode->method("__iter__", 0);
	size_t offset1 = bytecode->size();

	// see if we are at the end of the iteration
	bytecode->dup();
	bytecode->method("__end__", 0);
	size_t offset2 = bytecode->jumpTrue(0);

	// get the next iteration
	bytecode->dup();
	bytecode->method("__next__", 0);
	assignVariable(bytecode, name);

	// process the actual for loop block
	block(bytecode);

	// jump back to the start of the iteration
	bytecode->jump(offset1);

	// patch the completion jump
	bytecode->patchJump(offset2);

	// remove the two loop variables from the stack
	bytecode->pop(2);

	// cleanup scope stack
	popScope();
}

//
//	OtCompiler::ifStatement
//

void OtCompiler::ifStatement(OtByteCode bytecode) {
	// track various patches for jumps
	std::vector<size_t> patches;

	// handle if clause and block
	scanner.expect(OtScanner::IF_TOKEN);

	if (expression(bytecode)) {
		bytecode->method("__deref__", 0);
	}

	size_t offset = bytecode->jumpFalse(0);
	block(bytecode);

	patches.push_back(bytecode->jump(0));
	bytecode->patchJump(offset);

	// handle possible elsif clauses and their blocks
	while (scanner.matchToken(OtScanner::ELIF_TOKEN)) {
		scanner.advance();

		if (expression(bytecode)) {
			bytecode->method("__deref__", 0);
		}

		offset = bytecode->jumpFalse(0);
		block(bytecode);

		patches.push_back(bytecode->jump(0));
		bytecode->patchJump(offset);
	}

	// handle possible else clause and block
	if (scanner.matchToken(OtScanner::ELSE_TOKEN)) {
		scanner.advance();

		block(bytecode);
	}

	// patch all jumps
	for (auto const& patch : patches) {
		bytecode->patchJump(patch);
	}
}


//
//	OtCompiler::returnStatement
//

void OtCompiler::returnStatement(OtByteCode bytecode) {
	scanner.expect(OtScanner::RETURN_TOKEN);

	// get return value
	if (expression(bytecode)) {
		bytecode->method("__deref__", 0);
	}

	// cleanup stack (since we're doing a jump over all end of blocks)
	size_t locals = 0;

	for (auto i = scopeStack.rbegin(); i->type == BLOCK_SCOPE && i != scopeStack.rend(); i++) {
		locals += i->locals.size();
	}

	// no cleanup is required if we don't have any locals
	if (locals) {
		bytecode->move(locals);
		bytecode->pop(locals);
	}

	// the return value should now be top of the stack
	bytecode->exit();
	scanner.expect(OtScanner::SEMICOLON_TOKEN);
}


//
//	OtCompiler::throwStatement
//

void OtCompiler::throwStatement(OtByteCode bytecode) {
	scanner.expect(OtScanner::THROW_TOKEN);
	bytecode->push(OtThrow::create());

	if (expression(bytecode)) {
		bytecode->method("__deref__", 0);
	}

	bytecode->method("__call__", 1);
	scanner.expect(OtScanner::SEMICOLON_TOKEN);
}


//
//	OtCompiler::tryStatement
//

void OtCompiler::tryStatement(OtByteCode bytecode) {
	// handle "try" block
	scanner.expect(OtScanner::TRY_TOKEN);
	size_t offset1 = bytecode->pushTry();
	block(bytecode);
	bytecode->popTry();

	// jump around "catch" block
	size_t offset2 = bytecode->jump(0);
	bytecode->patchJump(offset1);

	// handle catch block
	scanner.expect(OtScanner::CATCH_TOKEN);
	scanner.expect(OtScanner::IDENTIFIER_TOKEN, false);
	std::string name = scanner.getText();

	// create a new block scope to handle error variable
	pushBlockScope();

	// declare error variable whose value is already on stack (courtesy of the VM)
	declareVariable(bytecode, name, true);
	scanner.advance();

	// compile "catch" block
	block(bytecode);

	// remove error variable from the stack
	bytecode->pop();

	// patch jump at the end of the "try" block
	bytecode->patchJump(offset2);

	// remove temporary scope
	popScope();
}


//
//	OtCompiler::whileStatement
//

void OtCompiler::whileStatement(OtByteCode bytecode) {
	scanner.expect(OtScanner::WHILE_TOKEN);
	size_t offset1 = bytecode->size();

	if (expression(bytecode)) {
		bytecode->method("__deref__", 0);
	}

	size_t offset2 = bytecode->jumpFalse(0);
	block(bytecode);
	bytecode->jump(offset1);
	bytecode->patchJump(offset2);
}


//
//	OtCompiler::statement
//

void OtCompiler::statement(OtByteCode bytecode) {
	// mark start of statement
	bytecode->mark(scanner.getTokenStart());

	// process statement
	switch (scanner.getToken()) {
		case OtScanner::LBRACE_TOKEN:
			block(bytecode);
			break;

		case OtScanner::VAR_TOKEN:
			variableDeclaration(bytecode);
			break;

		case OtScanner::CLASS_TOKEN:
			classDeclaration(bytecode);
			break;

		case OtScanner::FUNCTION_TOKEN:
			functionDeclaration(bytecode);
			break;

		case OtScanner::DO_TOKEN:
			doStatement(bytecode);
			break;

		case OtScanner::FOR_TOKEN:
			forStatement(bytecode);
			break;

		case OtScanner::IF_TOKEN:
			ifStatement(bytecode);
			break;

		case OtScanner::RETURN_TOKEN:
			returnStatement(bytecode);
			break;

		case OtScanner::THROW_TOKEN:
			throwStatement(bytecode);
			break;

		case OtScanner::TRY_TOKEN:
			tryStatement(bytecode);
			break;

		case OtScanner::WHILE_TOKEN:
			whileStatement(bytecode);
			break;

		default:
			expression(bytecode);
			bytecode->pop();
			scanner.expect(OtScanner::SEMICOLON_TOKEN);
			break;
	}

	// mark end of statement
	bytecode->mark(scanner.getLastTokenEnd());
}


//
//	OtCompiler::outputDisassembledBytecode
//

void OtCompiler::outputDisassembledBytecode(OtByteCode original, OtByteCode optimized) {
	std::cout << "Original:" << std::endl;
	std::cout << "---------" << std::endl << std::endl;
	std::cout << original->disassemble() << std::endl;
	std::cout << std::endl;

	std::cout << "Optimized:" << std::endl;
	std::cout << "----------" << std::endl << std::endl;
	std::cout << optimized->disassemble() << std::endl;
	std::cout << std::endl;
}
