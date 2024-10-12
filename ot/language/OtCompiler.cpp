//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "fmt/format.h"

#include "OtByteCodeFunction.h"
#include "OtCaptureReference.h"
#include "OtClass.h"
#include "OtClosure.h"
#include "OtCompiler.h"
#include "OtException.h"
#include "OtIdentifier.h"
#include "OtInteger.h"
#include "OtMemberReference.h"
#include "OtPathTools.h"
#include "OtReal.h"
#include "OtStackReference.h"
#include "OtString.h"
#include "OtThrow.h"
#include "OtVM.h"


//
//	OtCompiler::compileFile
//

OtByteCode OtCompiler::compileFile(const std::string &path) {
	// sanity check
	if (!OtPathExists(path)) {
		OtError("Can't open file [{}]", path);
	}

	// load source code and compile into bytecode
	std::ifstream stream(path);
	std::stringstream buffer;
	buffer << stream.rdbuf();
	stream.close();
	OtSource source = OtSourceClass::create(path, buffer.str());
	return compileSource(source, nullptr);
}


//
//	OtCompiler::compileText
//

OtByteCode OtCompiler::compileText(const std::string& text) {
	// convert to source object, compile and return bytecode
	OtSource source = OtSourceClass::create("__internal__", text);
	return compileSource(source, nullptr);
}


//
//	OtCompiler::compileSource
//

OtByteCode OtCompiler::compileSource(OtSource src, OtObject object) {
	// remember source code
	source = src;

	// load scanner
	scanner.loadSource(src);

	// setup global scope
	OtGlobal global = OtVM::getGlobal();
	pushObjectScope(nullptr, global);

	global->eachMemberID([this](OtID id) {
		declareVariable(id);
	});

	// setup bytecode
	OtByteCode bytecode = OtByteCode::create(src, OtIdentifier::create(OtPathGetStem(src->getModule())));

	// setup module scope (if required)
	if (object) {
		pushObjectScope(bytecode, object);

		object->eachMemberID([this](OtID id) {
			declareVariable(id);
		});
	}

	// process all statements
	while (!scanner.matchToken(OtScanner::EOS_TOKEN)) {
		statement(bytecode);
	}

	// clear object scope (if required)
	if (object) {
		popScope();
	}

	// clear global scope
	popScope();

	// sanity check
	OtAssert(scopeStack.size() == 0);

	// ensure we leave a default result on the stack
	bytecode->pushNull();

	// optimize code
	return optimizer.optimize(bytecode);
}


//
//	OtCompiler::compileExpression
//

OtByteCode OtCompiler::compileExpression(OtSource src) {
	// rememember source code
	source = src;

	// load scanner
	scanner.loadSource(src);

	// setup global scope
	OtGlobal global = OtVM::getGlobal();
	pushObjectScope(nullptr, global);

	global->eachMemberID([this](OtID id) {
		declareVariable(id);
	});

	// setup bytecode
	OtByteCode bytecode = OtByteCode::create(src, expressionID);

	// setup local scope
	pushBlockScope(bytecode);

	// process expression
	if (expression(bytecode)) {
		bytecode->method(dereferenceID, 0);
	}

	scanner.expect(OtScanner::EOS_TOKEN);

	// clear local and global scope
	popScope();
	popScope();

	// sanity check
	OtAssert(scopeStack.size() == 0);

	// optimize code
	return optimizer.optimize(bytecode);
}


//
//	OtCompiler::pushObjectScope
//

void OtCompiler::pushObjectScope(OtByteCode bytecode, OtObject object) {
	scopeStack.emplace_back(Scope(bytecode, object));
}


//
//	OtCompiler::pushFunctionScope
//

void OtCompiler::pushFunctionScope(OtByteCode bytecode) {
	scopeStack.emplace_back(Scope(bytecode));
}


//
//	OtCompiler::pushBlockScope
//

void OtCompiler::pushBlockScope(OtByteCode bytecode) {
	auto& scope = scopeStack.back();
	auto stackFrameOffset = scope.stackFrameOffset;

	if (scope.type == Scope::functionScope || scope.type == Scope::blockScope) {
		stackFrameOffset += scope.locals.size();
	}

	scopeStack.emplace_back(Scope(bytecode, stackFrameOffset));
}


//
//	OtCompiler::popScope
//

void OtCompiler::popScope() {
	// process all symbols (if required)
	auto& scope = scopeStack.back();

	if (scope.bytecode) {
		auto offset = scope.bytecode->size();

		for (auto& symbol : scope.symbols) {
			// close open symbol (if required)
			if (symbol.opcodeEnd == 0) {
				symbol.opcodeEnd = offset;
			}

			// add symbol to bytecode's symbol table
			scope.bytecode->addSymbol(symbol);
		}
	}

	// remove last scope
	scopeStack.pop_back();
}


//
//	OtCompiler::declareCapture
//

void OtCompiler::declareCapture(OtID id, OtStackItem item) {
	// find most recent function scope
	auto scope = scopeStack.rbegin();

	while (scope->type != Scope::functionScope && scope != scopeStack.rend()) {
		scope++;
	}

	// sanity check
	if (scope == scopeStack.rend()) {
		OtError("Internal error: no function scope on stack");
	}

	// see if this variable is already captured
	if (scope->captures.count(id)) {
		if (scope->captures[id] != item) {
			OtError("Internal error: captured variable [{}] has different stack offset", OtIdentifier::name(id));
		}

	} else {
		// add item to list of captured variables
		scope->captures[id] = item;
	}
}


//
//	OtCompiler::declareVariable
//

void OtCompiler::declareVariable(OtID id, bool alreadyOnStack) {
	// get current scope
	auto& scope = scopeStack.back();
	auto offset = scope.bytecode ? scope.bytecode->size() : 0;

	// avoid double declaration
	if (scope.locals.count(id)) {
		scanner.error(fmt::format("Variable [{}] already defined in this scope", OtIdentifier::name(id)));
	}

	// see if this variable obscures one referenced from a different scope
	if (scope.symbolIndex.count(id)) {
		// close the visibility of that variable
		scope.symbols[scope.symbolIndex[id]].opcodeEnd = offset;
	}

	// add variable to current scope
	if (scope.type == Scope::functionScope || scope.type == Scope::blockScope) {
		// variable lives on the stack
		auto slot = scope.stackFrameOffset + scope.locals.size();
		scope.locals[id] = slot;

		// add symbol to symbol table
		scope.symbolIndex[id] = scope.symbols.size();
		scope.symbols.emplace_back(id, slot, offset);

		// reserve space by putting a null object on the stack (if required)
		if (!alreadyOnStack) {
			scope.bytecode->pushNull();
		}

	} else {
		// variable lives on the heap
		scope.locals[id] = 0;

		// add symbol to symbol table
		scope.symbolIndex[id] = scope.symbols.size();
		scope.symbols.emplace_back(id, scope.object, offset);
	}
}


//
//	OtCompiler::resolveVariable
//

void OtCompiler::resolveVariable(OtID id, bool processSymbol) {
	// try to resolve name
	bool found = false;
	size_t functionLevel = 0;
	auto& scope = scopeStack.back();
	auto bytecode = scope.bytecode;

	// look at all scope levels in reverse order
	for (auto i = scopeStack.rbegin(); !found && i != scopeStack.rend(); i++) {
		// is variable in this scope?
		if (i->locals.count(id)) {
			// yes, handle different scope types
			switch(i->type) {
				case Scope::objectScope:
					// variable is object member
					bytecode->push(OtMemberReference::create(i->object, id));

					// process symbol if (required)
					if (processSymbol && !scope.symbolIndex.count(id)) {
						scope.symbols.emplace_back(id, i->object, statementStart);
					}

					break;

				case Scope::functionScope:
				case Scope::blockScope:
					if (functionLevel == 0) {
						// variable lives on the stack in the current stack frame
						bytecode->push(OtStackReference::create(id, i->locals[id]));

						// process symbol if (required)
						if (processSymbol && !scope.symbolIndex.count(id)) {
							scope.symbols.emplace_back(id, i->locals[id], statementStart);
						}

					} else {
						// variable is in an enclosing function, we need to capture it
						declareCapture(id, OtStackItem(functionLevel - 1, i->locals[id]));
						bytecode->push(OtCaptureReference::create(id));

						// process symbol if (required)
						if (processSymbol && !scope.symbolIndex.count(id)) {
							scope.symbols.emplace_back(id, statementStart);
						}
					}

					break;

				default:
					OtError("Internal Error: Invalid scope type");
			}

			found = true;
		}

		// do some housekeeping if we had a function scope
		if (!found && i->type == Scope::functionScope) {
			// track how many function levels we go back
			functionLevel++;
		}
	}

	// generate error if variable is not found
	if (!found) {
		scanner.error(fmt::format("Unknown variable [{}]", OtIdentifier::name(id)));
	}
}


//
//	OtCompiler::assignVariable
//

void OtCompiler::assignVariable(OtID id) {
	resolveVariable(id);

	auto bytecode = scopeStack.back().bytecode;
	bytecode->swap();
	bytecode->method(assignID, 1);
	bytecode->pop();
}


//
//	OtCompiler::function
//

void OtCompiler::function(OtByteCode bytecode, OtID id) {
	// each function has its own bytecode
	OtByteCode functionCode = OtByteCode::create(source, id);

	// start a new function scope
	pushFunctionScope(functionCode);

	// parse calling parameters
	scanner.expect(OtScanner::LPAREN_TOKEN);
	size_t count = 0;

	while (!scanner.matchToken(OtScanner::RPAREN_TOKEN) && !scanner.matchToken(OtScanner::EOS_TOKEN)) {
		scanner.expect(OtScanner::IDENTIFIER_TOKEN, false);
		declareVariable(scanner.getID(), true);
		scanner.advance();
		count++;

		if (!scanner.matchToken(OtScanner::RPAREN_TOKEN)) {
			scanner.expect(OtScanner::COMMA_TOKEN);
		}
	}

	scanner.expect(OtScanner::RPAREN_TOKEN);

	// get function level bytecode
	block(functionCode);

	// default return value in case function does not have return statement
	functionCode->pushNull();

	// create a new (optimized) bytecode function
	auto function = OtByteCodeFunction::create(optimizer.optimize(functionCode), count);

	// see if this function captures variables and needs a closure?
	auto& scope = scopeStack.back();

	if (scope.captures.size()) {
		// function does capture variables so let's wrap them in a closure
		bytecode->push(OtClosure::create(function, scope.captures));

		// generate code to perform the actual capture
		bytecode->method(captureID, 0);

	} else {
		// no captures so just put new function on the stack
		bytecode->push(function);
	}

	// end function scope
	popScope();
}


//
//	OtCompiler::super
//

void OtCompiler::super(OtByteCode bytecode) {
	// ensure we are in a class definition
	if (!classStack.size()) {
		OtError("Can't use [super] outside of a class definition");
	}

	// put class reference on stack
	bytecode->push(classStack.back());

	// parse member reference and create super instruction
	scanner.expect(OtScanner::PERIOD_TOKEN);
	scanner.expect(OtScanner::IDENTIFIER_TOKEN, false);
	bytecode->super(scanner.getID());
	scanner.advance();
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
			function(bytecode, anonymousID);
			reference = false;
			break;

		case OtScanner::SUPER_TOKEN:
			// handle superclass construct
			scanner.advance();
			super(bytecode);
			reference = false;
			break;

		case OtScanner::IDENTIFIER_TOKEN: {
			// handle named reference
			resolveVariable(scanner.getID());
			scanner.advance();
			reference = true;
			break;
		}

		case OtScanner::LBRACKET_TOKEN:
			// handle array constant
			scanner.advance();
			resolveVariable(arrayID, false);
			bytecode->method(dereferenceID, 0);
			bytecode->method(callID, 0);
			bytecode->dup();

			if (scanner.getToken() != OtScanner::RBRACKET_TOKEN) {
				bytecode->method(initID, expressions(bytecode));
			}

			bytecode->pop();
			scanner.expect(OtScanner::RBRACKET_TOKEN);
			reference = false;
			break;

		case OtScanner::LBRACE_TOKEN:
			// handle dictionary constant
			scanner.advance();
			resolveVariable(dictID, false);
			bytecode->method(dereferenceID, 0);
			bytecode->method(callID, 0);
			bytecode->dup();

			while (scanner.getToken() != OtScanner::RBRACE_TOKEN && scanner.getToken() != OtScanner::EOS_TOKEN) {
				scanner.expect(OtScanner::STRING_TOKEN, false);
				bytecode->push(OtString::create(scanner.getString()));
				scanner.advance();
				scanner.expect(OtScanner::COLON_TOKEN);

				if (expression(bytecode)) {
					bytecode->method(dereferenceID, 0);
				}

				count += 2;

				if (scanner.getToken() == OtScanner::COMMA_TOKEN) {
					scanner.advance();
				}
			}

			scanner.expect(OtScanner::RBRACE_TOKEN);

			if (count) {
				bytecode->method(initID, count);
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
		size_t count;

		// generate code
		switch (token) {
			case OtScanner::LBRACKET_TOKEN:
				// index object
				if (reference) {
					bytecode->method(dereferenceID, 0);
				}

				if (expression(bytecode)) {
					bytecode->method(dereferenceID, 0);
				}

				scanner.expect(OtScanner::RBRACKET_TOKEN);
				bytecode->method(indexID, 1);
				reference = true;
				break;

			case OtScanner::LPAREN_TOKEN:
				// call object
				if (reference) {
					bytecode->method(dereferenceID, 0);
				}

				count = scanner.matchToken(OtScanner::RPAREN_TOKEN) ? 0 : expressions(bytecode);
				scanner.expect(OtScanner::RPAREN_TOKEN);
				bytecode->method(callID, count);
				reference = false;
				break;

			case OtScanner::PERIOD_TOKEN:
				// member access
				if (reference) {
					bytecode->method(dereferenceID, 0);
				}

				scanner.expect(OtScanner::IDENTIFIER_TOKEN, false);
				bytecode->member(scanner.getID());
				scanner.advance();
				reference = true;
				break;

			case OtScanner::INCREMENT_TOKEN:
				if (!reference) {
					scanner.error("Lvalue required for '++'");
				}

				bytecode->dup();
				bytecode->method(dereferenceID, 0);
				bytecode->swap();
				bytecode->dup();
				bytecode->method(dereferenceID, 0);
				bytecode->method(incrementID, 0);
				bytecode->method(assignID, 1);
				bytecode->pop();
				reference = false;
				break;

			case OtScanner::DECREMENT_TOKEN:
				if (!reference) {
					scanner.error("Lvalue required for '--'");
				}

				bytecode->dup();
				bytecode->method(dereferenceID, 0);
				bytecode->swap();
				bytecode->dup();
				bytecode->method(dereferenceID, 0);
				bytecode->method(decrementID, 0);
				bytecode->method(assignID, 1);
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
					bytecode->method(dereferenceID, 0);
				}

				bytecode->method(negateID, 0);
				break;

		case OtScanner::ADD_TOKEN:
			if (reference) {
				bytecode->method(dereferenceID, 0);
			}

			bytecode->method(plusID, 0);
			break;

			case OtScanner::NEGATE_TOKEN:
				if (reference) {
					bytecode->method(dereferenceID, 0);
				}

				bytecode->method(logicalNotID, 0);
				break;

			case OtScanner::BITWISE_NOT_TOKEN:
				if (reference) {
					bytecode->method(dereferenceID, 0);
				}

				bytecode->method(bitwiseNotID, 0);
				break;

			case OtScanner::INCREMENT_TOKEN:
				if (!reference) {
					scanner.error("Lvalue required for '++'");
				}

				bytecode->dup();
				bytecode->method(dereferenceID, 0);
				bytecode->method(incrementID, 0);
				bytecode->method(assignID, 1);
				break;

			case OtScanner::DECREMENT_TOKEN:
				if (!reference) {
					scanner.error("Lvalue required for '--'");
				}

				bytecode->dup();
				bytecode->method(dereferenceID, 0);
				bytecode->method(decrementID, 0);
				bytecode->method(assignID, 1);
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
			bytecode->method(dereferenceID, 0);
		}

		// parse right side
		if (prefix(bytecode)) {
			bytecode->method(dereferenceID, 0);
		}

		// generate code
		switch (token) {
			case OtScanner::MULTIPLY_TOKEN:
				bytecode->method(multiplyID, 1);
				break;

			case OtScanner::DIVIDE_TOKEN:
				bytecode->method(divideID, 1);
				break;

			case OtScanner::POWER_TOKEN:
				bytecode->method(powerID, 1);
				break;

			case OtScanner::MODULO_TOKEN:
				bytecode->method(moduloID, 1);
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
			bytecode->method(dereferenceID, 0);
		}

		// parse right side
		if (multiplications(bytecode)) {
			bytecode->method(dereferenceID, 0);
		}

		// generate code
		if (token == OtScanner::ADD_TOKEN) {
			bytecode->method(addID, 1);

		} else {
			bytecode->method(subtractID, 1);
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
			bytecode->method(dereferenceID, 0);
		}

		// parse right side
		if (additions(bytecode)) {
			bytecode->method(dereferenceID, 0);
		}

		// generate code
		if (token == OtScanner::SHIFT_LEFT_TOKEN) {
			bytecode->method(leftShiftID, 1);

		} else {
			bytecode->method(rightShiftID, 1);
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
			bytecode->method(dereferenceID, 0);
		}

		// parse right side
		if (shift(bytecode)) {
			bytecode->method(dereferenceID, 0);
		}

		// generate code
		switch (token) {
			case OtScanner::LESS_TOKEN:
				bytecode->method(lessThenID, 1);
				break;

			case OtScanner::LESS_EQUAL_TOKEN:
				bytecode->method(lessEqualID, 1);
				break;

			case OtScanner::GREATER_TOKEN:
				bytecode->method(greaterThenID, 1);
				break;

			case OtScanner::GREATER_EQUAL_TOKEN:
				bytecode->method(greaterEqualID, 1);
				break;

			case OtScanner::IN_TOKEN:
				bytecode->swap();
				bytecode->method(containsID, 1);
				break;

			case OtScanner::NOT_TOKEN:
				bytecode->swap();
				bytecode->method(containsID, 1);
				bytecode->method(logicalNotID, 0);
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
			bytecode->method(dereferenceID, 0);
		}

		// parse right side
		if (relation(bytecode)) {
			bytecode->method(dereferenceID, 0);
		}

		// generate code
		if (token == OtScanner::EQUAL_TOKEN) {
			bytecode->method(equalID, 1);

		} else {
			bytecode->method(notEqualID, 1);
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
			bytecode->method(dereferenceID, 0);
		}

		// parse right side
		if (equal(bytecode)) {
			bytecode->method(dereferenceID, 0);
		}

		// generate code
		bytecode->method(bitwiseAndID, 1);
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
			bytecode->method(dereferenceID, 0);
		}

		// parse right side
		if (bitwiseAnd(bytecode)) {
			bytecode->method(dereferenceID, 0);
		}

		// generate code
		bytecode->method(bitwiseXorID, 1);
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
			bytecode->method(dereferenceID, 0);
		}

		// parse right side
		if (bitwiseXor(bytecode)) {
			bytecode->method(dereferenceID, 0);
		}

		// generate code
		bytecode->method(bitwiseOrID, 1);
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
			bytecode->method(dereferenceID, 0);
		}

		// parse right side
		if (bitwiseOr(bytecode)) {
			bytecode->method(dereferenceID, 0);
		}

		// generate code
		bytecode->method(logicalAndID, 1);
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
			bytecode->method(dereferenceID, 0);
		}

		// parse right side
		if (andExpression(bytecode)) {
			bytecode->method(dereferenceID, 0);
		}

		// generate code
		bytecode->method(logicalOrID, 1);
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
			bytecode->method(dereferenceID, 0);
		}

		// evaluate expression
		size_t offset1 = bytecode->jumpFalse(0);

		// process "true" expression
		if (expression(bytecode)) {
			bytecode->method(dereferenceID, 0);
		}

		scanner.expect(OtScanner::COLON_TOKEN);

		// jump around "false" expression and patch first jump
		size_t offset2 = bytecode->jump(0);
		bytecode->patchJump(offset1);

		// process "false" expression
		if (expression(bytecode)) {
			bytecode->method(dereferenceID, 0);
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
			bytecode->method(dereferenceID, 0);
		}

		// parse right side
		scanner.advance();

		if (expression(bytecode)) {
			bytecode->method(dereferenceID, 0);
		}

		// handle different assignments
		switch (token) {
			case OtScanner::MULTIPLY_ASSIGNMENT_TOKEN:
				bytecode->method(multiplyID, 1);
				break;

			case OtScanner::DIVIDE_ASSIGNMENT_TOKEN:
				bytecode->method(divideID, 1);
				break;

			case OtScanner::MODULO_ASSIGNMENT_TOKEN:
				bytecode->method(moduloID, 1);
				break;

			case OtScanner::ADD_ASSIGNMENT_TOKEN:
				bytecode->method(addID, 1);
				break;

			case OtScanner::SUBTRACT_ASSIGNMENT_TOKEN:
				bytecode->method(subtractID, 1);
				break;

			case OtScanner::SHIFT_LEFT_ASSIGNMENT_TOKEN:
				bytecode->method(leftShiftID, 1);
				break;

			case OtScanner::SHIFT_RIGHT_ASSIGNMENT_TOKEN:
				bytecode->method(rightShiftID, 1);
				break;

			case OtScanner::BITWISE_AND_ASSIGNMENT_TOKEN:
				bytecode->method(bitwiseAndID, 1);
				break;

			case OtScanner::BITWISE_OR_ASSIGNMENT_TOKEN:
				bytecode->method(bitwiseOrID, 1);
				break;

			case OtScanner::BITWISE_XOR_ASSIGNMENT_TOKEN:
				bytecode->method(bitwiseXorID, 1);
				break;
		}

		// perform assignment
		bytecode->method(assignID, 1);
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
		bytecode->method(dereferenceID, 0);
	}

	while (scanner.matchToken(OtScanner::COMMA_TOKEN)) {
		scanner.advance();

		if (expression(bytecode)) {
			bytecode->method(dereferenceID, 0);
		}

		count++;
	}

	return count;
}


//
//	OtCompiler::block
//

void OtCompiler::block(OtByteCode bytecode) {
	// enter a new block scope
	scanner.expect(OtScanner::LBRACE_TOKEN);
	pushBlockScope(bytecode);

	// process all statements in a block
	while (!scanner.matchToken(OtScanner::RBRACE_TOKEN) && !scanner.matchToken(OtScanner::EOS_TOKEN)) {
		statement(bytecode);
	}

	scanner.expect(OtScanner::RBRACE_TOKEN);

	// locals are going out of scope so we remove them from the stack (if required)
	auto locals = scopeStack.back().locals.size();

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
	auto id = scanner.getID();
	scanner.advance();

	// add variable to scope
	declareVariable(id);

	// process initial value (if required)
	if (scanner.matchToken(OtScanner::ASSIGNMENT_TOKEN)) {
		// process expression
		scanner.advance();

		if (expression(bytecode)) {
			bytecode->method(dereferenceID, 0);
		}

	} else {
		// use null as default value
		bytecode->pushNull();
	}

	// assign value to variable
	assignVariable(id);

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
	auto id = scanner.getID();

	// create new class
	OtClass cls = OtClass::create(id);
	classStack.push_back(cls);

	// add class to current scope
	declareVariable(id);
	bytecode->push(cls);
	assignVariable(id);
	scanner.advance();

	// handle parent class
	scanner.expect(OtScanner::COLON_TOKEN);
	bytecode->push(cls);

	if (expression(bytecode)) {
		bytecode->method(dereferenceID, 0);
	}

	bytecode->method(parentID, 1);
	bytecode->pop();

	// start new class scope
	pushObjectScope(bytecode, cls);

	// process class content
	scanner.expect(OtScanner::LBRACE_TOKEN);

	while (!scanner.matchToken(OtScanner::RBRACE_TOKEN) && !scanner.matchToken(OtScanner::EOS_TOKEN)) {
		statement(bytecode);
	}

	scanner.expect(OtScanner::RBRACE_TOKEN);

	// end class scope
	popScope();
	classStack.pop_back();
}


//
//	OtCompiler::functionDeclaration
//

void OtCompiler::functionDeclaration(OtByteCode bytecode) {
	// get function name
	scanner.expect(OtScanner::FUNCTION_TOKEN);
	scanner.expect(OtScanner::IDENTIFIER_TOKEN, false);
	auto id = scanner.getID();

	// add function to scope to allow for recursion
	declareVariable(id);
	scanner.advance();

	// parse function definition to function object on stack
	function(bytecode, id);

	// assign function object to variable name
	assignVariable(id);
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
		bytecode->method(dereferenceID, 0);
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
	pushBlockScope(bytecode);

	// get name of iteration variable
	scanner.expect(OtScanner::IDENTIFIER_TOKEN, false);
	auto id = scanner.getID();

	// create loop variable on the stack
	declareVariable(id);

	// get the object to be iterated on
	scanner.advance();
	scanner.expect(OtScanner::IN_TOKEN);

	if (expression(bytecode)) {
		bytecode->method(dereferenceID, 0);
	}

	// pretend iterator is a local variable (that can't be addressed)
	declareVariable(blankID, true);

	// turn object into an iterator
	bytecode->method(iteratorID, 0);
	size_t offset1 = bytecode->size();

	// see if we are at the end of the iteration
	bytecode->dup();
	bytecode->method(endID, 0);
	size_t offset2 = bytecode->jumpTrue(0);

	// get the next iteration
	bytecode->dup();
	bytecode->method(nextID, 0);
	assignVariable(id);

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
		bytecode->method(dereferenceID, 0);
	}

	size_t offset = bytecode->jumpFalse(0);
	block(bytecode);

	patches.push_back(bytecode->jump(0));
	bytecode->patchJump(offset);

	// handle possible elsif clauses and their blocks
	while (scanner.matchToken(OtScanner::ELIF_TOKEN)) {
		scanner.advance();

		if (expression(bytecode)) {
			bytecode->method(dereferenceID, 0);
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
		bytecode->method(dereferenceID, 0);
	}

	// cleanup stack (since we're doing a jump over all end of blocks)
	size_t locals = 0;

	for (auto i = scopeStack.rbegin(); i->type == Scope::blockScope && i != scopeStack.rend(); i++) {
		locals += i->locals.size();
	}

	// no cleanup is required if we don't have any locals
	if (locals) {
		// move up result past the locals and shorten stack
		bytecode->move(locals);
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
		bytecode->method(dereferenceID, 0);
	}

	bytecode->method(callID, 1);
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
	auto id = scanner.getID();

	// create a new block scope to handle error variable
	pushBlockScope(bytecode);

	// declare error variable whose value is already on stack (courtesy of the VM)
	declareVariable(id, true);
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
		bytecode->method(dereferenceID, 0);
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
	// get start of statement information
	auto sourceStart = scanner.getTokenStart();
	auto opcodeStart = bytecode->size();
	statementStart = opcodeStart;

	// mark start of statement in bytecode
	bytecode->statement();

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

	// register statement
	bytecode->addStatement(
		sourceStart, scanner.getTokenStart(),
		opcodeStart, bytecode->size());
}
