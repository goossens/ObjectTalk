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
#include "OtPath.h"
#include "OtReal.h"
#include "OtStackReference.h"
#include "OtString.h"
#include "OtThrow.h"
#include "OtVM.h"


//
//	OtCompiler::compileFile
//

OtByteCode OtCompiler::compileFile(const std::string& path) {
	// sanity check
	if (!OtPath::exists(path)) {
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
	OtByteCode bytecode = OtByteCode::create(src, OtIdentifier::create(OtPath::getStem(src->getModule())));

	// setup module scope (if required)
	if (object) {
		pushObjectScope(bytecode, object);

		object->eachMemberID([this](OtID id) {
			declareVariable(id);
		});
	}

	// process all statements
	while (!scanner.matchToken(OtScanner::Token::endOfText)) {
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

	scanner.expect(OtScanner::Token::endOfText);

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

	if (scope.type == Scope::Type::function || scope.type == Scope::Type::block) {
		stackFrameOffset += scope.locals.size();
	}

	scopeStack.emplace_back(Scope(bytecode, stackFrameOffset));
}


//
//	OtCompiler::closeScope
//

void OtCompiler::closeScope() {
	// process all symbols in this scope (if required)
	auto& scope = scopeStack.back();

	if (scope.bytecode && !scope.closed) {
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

	scope.closed = true;
}


//
//	OtCompiler::popScope
//

void OtCompiler::popScope() {
	// close and remove last scope
	closeScope();
	scopeStack.pop_back();
}


//
//	OtCompiler::declareCapture
//

void OtCompiler::declareCapture(OtID id, std::pair<size_t, size_t> item) {
	// find most recent function scope
	auto scope = scopeStack.rbegin();

	while (scope->type != Scope::Type::function && scope != scopeStack.rend()) {
		scope++;
	}

	// sanity check
	if (scope == scopeStack.rend()) {
		OtError("Internal error: no function scope on stack");
	}

	// add item to list of captured variables (if not already captured)
	if (scope->captures.count(id) == 0) {
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
	if (scope.type == Scope::Type::function || scope.type == Scope::Type::block) {
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
				case Scope::Type::object:
					// variable is object member
					bytecode->push(OtMemberReference::create(i->object, id));

					// process symbol if (required)
					if (processSymbol && !scope.symbolIndex.count(id)) {
						scope.symbols.emplace_back(id, i->object, statementStart);
					}

					break;

				case Scope::Type::function:
				case Scope::Type::block:
					if (functionLevel == 0) {
						// variable lives on the stack in the current stack frame
						bytecode->push(OtStackReference::create(id, i->locals[id]));

						// process symbol if (required)
						if (processSymbol && !scope.symbolIndex.count(id)) {
							scope.symbols.emplace_back(id, i->locals[id], statementStart);
						}

					} else {
						// variable is in an enclosing function, we need to capture it
						declareCapture(id, {functionLevel - 1, i->locals[id]});
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
		if (!found && i->type == Scope::Type::function) {
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
	scanner.expect(OtScanner::Token::leftParenthesis);
	size_t count = 0;

	while (!scanner.matchToken(OtScanner::Token::rightParenthesis) && !scanner.matchToken(OtScanner::Token::endOfText)) {
		scanner.expect(OtScanner::Token::identifier, false);
		declareVariable(scanner.getID(), true);
		scanner.advance();
		count++;

		if (!scanner.matchToken(OtScanner::Token::rightParenthesis)) {
			scanner.expect(OtScanner::Token::comma);
		}
	}

	scanner.expect(OtScanner::Token::rightParenthesis);

	// get function level bytecode
	block(functionCode);

	// close out local symbols now so the optimizer calculates their visibility ranges correctly
	closeScope();

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
	scanner.expect(OtScanner::Token::period);
	scanner.expect(OtScanner::Token::identifier, false);
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
		case OtScanner::Token::leftParenthesis:
			// handle sub-expression
			scanner.advance();
			reference = expression(bytecode);
			scanner.expect(OtScanner::Token::rightParenthesis);
			break;

		case OtScanner::Token::integerLiteral:
			// handle integer constants
			bytecode->push(OtInteger::create(scanner.getInteger()));
			scanner.advance();
			reference = false;
			break;

		case OtScanner::Token::realLiteral:
			// handle real constants
			bytecode->push(OtReal::create(scanner.getReal()));
			scanner.advance();
			reference = false;
			break;

		case OtScanner::Token::stringLiteral:
			// handle string constants
			bytecode->push(OtString::create(scanner.getString()));
			scanner.advance();
			reference = false;
			break;

		case OtScanner::Token::functionKeyword:
			// handle function definition
			scanner.advance();
			function(bytecode, anonymousID);
			reference = false;
			break;

		case OtScanner::Token::superKeyword:
			// handle superclass construct
			scanner.advance();
			super(bytecode);
			reference = false;
			break;

		case OtScanner::Token::identifier: {
			// handle named reference
			resolveVariable(scanner.getID());
			scanner.advance();
			reference = true;
			break;
		}

		case OtScanner::Token::leftBracket:
			// handle array constant
			scanner.advance();
			resolveVariable(arrayID, false);
			bytecode->method(dereferenceID, 0);
			bytecode->method(callID, 0);
			bytecode->dup();

			if (scanner.getToken() != OtScanner::Token::rightBracket) {
				bytecode->method(initID, expressions(bytecode));
			}

			bytecode->pop();
			scanner.expect(OtScanner::Token::rightBracket);
			reference = false;
			break;

		case OtScanner::Token::leftBrace:
			// handle dictionary constant
			scanner.advance();
			resolveVariable(dictID, false);
			bytecode->method(dereferenceID, 0);
			bytecode->method(callID, 0);
			bytecode->dup();

			while (scanner.getToken() != OtScanner::Token::rightBrace && scanner.getToken() != OtScanner::Token::endOfText) {
				scanner.expect(OtScanner::Token::stringLiteral, false);
				bytecode->push(OtString::create(scanner.getString()));
				scanner.advance();
				scanner.expect(OtScanner::Token::colon);

				if (expression(bytecode)) {
					bytecode->method(dereferenceID, 0);
				}

				count += 2;

				if (scanner.getToken() == OtScanner::Token::comma) {
					scanner.advance();
				}
			}

			scanner.expect(OtScanner::Token::rightBrace);

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
	OtScanner::Token token = scanner.getToken();

	// process postfix
	while (token == OtScanner::Token::leftBracket ||
		   token == OtScanner::Token::leftParenthesis ||
		   token == OtScanner::Token::period ||
		   token == OtScanner::Token::incrementOperator ||
		   token == OtScanner::Token::decrementOperator) {
		scanner.advance();
		size_t count;

		// generate code
		switch (token) {
			case OtScanner::Token::leftBracket:
				// index object
				if (reference) {
					bytecode->method(dereferenceID, 0);
				}

				if (expression(bytecode)) {
					bytecode->method(dereferenceID, 0);
				}

				scanner.expect(OtScanner::Token::rightBracket);
				bytecode->method(indexID, 1);
				reference = true;
				break;

			case OtScanner::Token::leftParenthesis:
				// call object
				if (reference) {
					bytecode->method(dereferenceID, 0);
				}

				count = scanner.matchToken(OtScanner::Token::rightParenthesis) ? 0 : expressions(bytecode);
				scanner.expect(OtScanner::Token::rightParenthesis);
				bytecode->method(callID, count);
				reference = false;
				break;

			case OtScanner::Token::period:
				// member access
				if (reference) {
					bytecode->method(dereferenceID, 0);
				}

				scanner.expect(OtScanner::Token::identifier, false);
				bytecode->member(scanner.getID());
				scanner.advance();
				reference = true;
				break;

			case OtScanner::Token::incrementOperator:
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

			case OtScanner::Token::decrementOperator:
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

			default:
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
	OtScanner::Token token = scanner.getToken();

	// process prefix
	if (token == OtScanner::Token::subtractOperator ||
		token == OtScanner::Token::negateOperator ||
		token == OtScanner::Token::bitwiseNotOperator ||
		token == OtScanner::Token::incrementOperator ||
		token == OtScanner::Token::decrementOperator) {
		scanner.advance();
		bool reference = postfix(bytecode);

		// generate code
		switch (token) {
			case OtScanner::Token::subtractOperator:
				if (reference) {
					bytecode->method(dereferenceID, 0);
				}

				bytecode->method(negateID, 0);
				break;

		case OtScanner::Token::addOperator:
			if (reference) {
				bytecode->method(dereferenceID, 0);
			}

			bytecode->method(plusID, 0);
			break;

			case OtScanner::Token::negateOperator:
				if (reference) {
					bytecode->method(dereferenceID, 0);
				}

				bytecode->method(logicalNotID, 0);
				break;

			case OtScanner::Token::bitwiseNotOperator:
				if (reference) {
					bytecode->method(dereferenceID, 0);
				}

				bytecode->method(bitwiseNotID, 0);
				break;

			case OtScanner::Token::incrementOperator:
				if (!reference) {
					scanner.error("Lvalue required for '++'");
				}

				bytecode->dup();
				bytecode->method(dereferenceID, 0);
				bytecode->method(incrementID, 0);
				bytecode->method(assignID, 1);
				break;

			case OtScanner::Token::decrementOperator:
				if (!reference) {
					scanner.error("Lvalue required for '--'");
				}

				bytecode->dup();
				bytecode->method(dereferenceID, 0);
				bytecode->method(decrementID, 0);
				bytecode->method(assignID, 1);
				break;

			default:
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
	OtScanner::Token token = scanner.getToken();

	while (token == OtScanner::Token::multiplyOperator ||
		   token == OtScanner::Token::divideOperator ||
		   token == OtScanner::Token::powerOperator ||
		   token == OtScanner::Token::moduloOperator) {
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
			case OtScanner::Token::multiplyOperator:
				bytecode->method(multiplyID, 1);
				break;

			case OtScanner::Token::divideOperator:
				bytecode->method(divideID, 1);
				break;

			case OtScanner::Token::powerOperator:
				bytecode->method(powerID, 1);
				break;

			case OtScanner::Token::moduloOperator:
				bytecode->method(moduloID, 1);
				break;

			default:
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
	OtScanner::Token token = scanner.getToken();

	while (token == OtScanner::Token::addOperator || token == OtScanner::Token::subtractOperator) {
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
		if (token == OtScanner::Token::addOperator) {
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
	OtScanner::Token token = scanner.getToken();

	while (token == OtScanner::Token::shiftLeftOperator || token == OtScanner::Token::shiftRightOperator) {
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
		if (token == OtScanner::Token::shiftLeftOperator) {
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
	OtScanner::Token token = scanner.getToken();

	while (token == OtScanner::Token::lessOperator ||
		   token == OtScanner::Token::lessEqualOperator ||
		   token == OtScanner::Token::greaterOperator ||
		   token == OtScanner::Token::greaterEqualOperator ||
		   token == OtScanner::Token::inKeyword ||
		   token == OtScanner::Token::notKeyword) {
		scanner.advance();

		// expect "in" if required
		if (token == OtScanner::Token::notKeyword) {
			scanner.expect(OtScanner::Token::inKeyword);
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
			case OtScanner::Token::lessOperator:
				bytecode->method(lessThenID, 1);
				break;

			case OtScanner::Token::lessEqualOperator:
				bytecode->method(lessEqualID, 1);
				break;

			case OtScanner::Token::greaterOperator:
				bytecode->method(greaterThenID, 1);
				break;

			case OtScanner::Token::greaterEqualOperator:
				bytecode->method(greaterEqualID, 1);
				break;

			case OtScanner::Token::inKeyword:
				bytecode->swap();
				bytecode->method(containsID, 1);
				break;

			case OtScanner::Token::notKeyword:
				bytecode->swap();
				bytecode->method(containsID, 1);
				bytecode->method(logicalNotID, 0);
				break;

			default:
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
	OtScanner::Token token = scanner.getToken();

	while (token == OtScanner::Token::equalOperator || token == OtScanner::Token::notEqualOperator) {
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
		if (token == OtScanner::Token::equalOperator) {
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
	if (scanner.matchToken(OtScanner::Token::bitwiseAndOperator)) {
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
	if (scanner.matchToken(OtScanner::Token::bitwiseXorOperator)) {
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
	if (scanner.matchToken(OtScanner::Token::bitwiseOrOperator)) {
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
	while (scanner.matchToken(OtScanner::Token::andOperator)) {
		// skip token
		scanner.advance();

		// deref if required
		if (reference) {
			bytecode->method(dereferenceID, 0);
		}

		// see if we can short-circuit the evaluation
		bytecode->dup();
		size_t offset = bytecode->jumpFalse(0);

		// parse right side
		if (bitwiseOr(bytecode)) {
			bytecode->method(dereferenceID, 0);
		}

		// perform logical AND
		bytecode->method(logicalAndID, 1);

		// patch shortcut path
		bytecode->patchJump(offset);

		// item on stack is now a value
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
	while (scanner.matchToken(OtScanner::Token::orOperator)) {
		// skip token
		scanner.advance();

		// deref if required
		if (reference) {
			bytecode->method(dereferenceID, 0);
		}

		// see if we can short-circuit the evaluation
		bytecode->dup();
		size_t offset = bytecode->jumpTrue(0);

		// parse right side
		if (andExpression(bytecode)) {
			bytecode->method(dereferenceID, 0);
		}

		// perform logical OR
		bytecode->method(logicalOrID, 1);

		// patch shortcut path
		bytecode->patchJump(offset);

		// item on stack is now a value
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

	if (scanner.matchToken(OtScanner::Token::questionMark)) {
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

		scanner.expect(OtScanner::Token::colon);

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
	OtScanner::Token token = scanner.getToken();

	// process all assignment operators
	while (token == OtScanner::Token::assignment ||
		   token == OtScanner::Token::multiplyAssignment ||
		   token == OtScanner::Token::divideAssignment ||
		   token == OtScanner::Token::moduloAssignment ||
		   token == OtScanner::Token::addAssignment ||
		   token == OtScanner::Token::subtractAssignment ||
		   token == OtScanner::Token::shiftLeftAssignment ||
		   token == OtScanner::Token::shiftRightAssignment ||
		   token == OtScanner::Token::bitwiseAndAssignment ||
		   token == OtScanner::Token::bitwiseOrAssignment ||
		   token == OtScanner::Token::bitwiseXorAssignment) {
		// sanity check
		if (!reference) {
			scanner.error("Lvalue required for assignments");
		}

		// duplicate left side if required
		if (token != OtScanner::Token::assignment) {
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
			case OtScanner::Token::multiplyAssignment:
				bytecode->method(multiplyID, 1);
				break;

			case OtScanner::Token::divideAssignment:
				bytecode->method(divideID, 1);
				break;

			case OtScanner::Token::moduloAssignment:
				bytecode->method(moduloID, 1);
				break;

			case OtScanner::Token::addAssignment:
				bytecode->method(addID, 1);
				break;

			case OtScanner::Token::subtractAssignment:
				bytecode->method(subtractID, 1);
				break;

			case OtScanner::Token::shiftLeftAssignment:
				bytecode->method(leftShiftID, 1);
				break;

			case OtScanner::Token::shiftRightAssignment:
				bytecode->method(rightShiftID, 1);
				break;

			case OtScanner::Token::bitwiseAndAssignment:
				bytecode->method(bitwiseAndID, 1);
				break;

			case OtScanner::Token::bitwiseOrAssignment:
				bytecode->method(bitwiseOrID, 1);
				break;

			case OtScanner::Token::bitwiseXorAssignment:
				bytecode->method(bitwiseXorID, 1);
				break;

			default:
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

	while (scanner.matchToken(OtScanner::Token::comma)) {
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
	scanner.expect(OtScanner::Token::leftBrace);
	pushBlockScope(bytecode);

	// process all statements in a block
	while (!scanner.matchToken(OtScanner::Token::rightBrace) && !scanner.matchToken(OtScanner::Token::endOfText)) {
		statement(bytecode);
	}

	scanner.expect(OtScanner::Token::rightBrace);

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
	scanner.expect(OtScanner::Token::varKeyword);
	scanner.expect(OtScanner::Token::identifier, false);
	auto id = scanner.getID();
	scanner.advance();

	// add variable to scope
	declareVariable(id);

	// process initial value (if required)
	if (scanner.matchToken(OtScanner::Token::assignment)) {
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
	scanner.expect(OtScanner::Token::colon);
}


//
//	OtCompiler::classDeclaration
//

void OtCompiler::classDeclaration(OtByteCode bytecode) {
	// handle class name
	scanner.expect(OtScanner::Token::classKeyword);
	scanner.expect(OtScanner::Token::identifier, false);
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
	scanner.expect(OtScanner::Token::colon);
	bytecode->push(cls);

	if (expression(bytecode)) {
		bytecode->method(dereferenceID, 0);
	}

	bytecode->method(parentID, 1);
	bytecode->pop();

	// start new class scope
	pushObjectScope(bytecode, cls);

	// process class content
	scanner.expect(OtScanner::Token::leftBrace);

	while (!scanner.matchToken(OtScanner::Token::rightBrace) && !scanner.matchToken(OtScanner::Token::endOfText)) {
		statement(bytecode);
	}

	scanner.expect(OtScanner::Token::rightBrace);

	// end class scope
	popScope();
	classStack.pop_back();
}


//
//	OtCompiler::functionDeclaration
//

void OtCompiler::functionDeclaration(OtByteCode bytecode) {
	// get function name
	scanner.expect(OtScanner::Token::functionKeyword);
	scanner.expect(OtScanner::Token::identifier, false);
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
	scanner.expect(OtScanner::Token::doKeyword);
	size_t offset = bytecode->size();

	block(bytecode);

	scanner.expect(OtScanner::Token::whileKeyword);

	if (expression(bytecode)) {
		bytecode->method(dereferenceID, 0);
	}

	bytecode->jumpTrue(offset);
	scanner.expect(OtScanner::Token::colon);
}


//
//	OtCompiler::forStatement
//

void OtCompiler::forStatement(OtByteCode bytecode) {
	scanner.expect(OtScanner::Token::forKeyword);

	// "for loop" is "fake" block to handle loop variables
	pushBlockScope(bytecode);

	// get name of iteration variable
	scanner.expect(OtScanner::Token::identifier, false);
	auto id = scanner.getID();

	// create loop variable on the stack
	declareVariable(id);

	// get the object to be iterated on
	scanner.advance();
	scanner.expect(OtScanner::Token::inKeyword);

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
	scanner.expect(OtScanner::Token::ifKeyword);

	if (expression(bytecode)) {
		bytecode->method(dereferenceID, 0);
	}

	size_t offset = bytecode->jumpFalse(0);
	block(bytecode);

	patches.push_back(bytecode->jump(0));
	bytecode->patchJump(offset);

	// handle possible elsif clauses and their blocks
	while (scanner.matchToken(OtScanner::Token::elifKeyword)) {
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
	if (scanner.matchToken(OtScanner::Token::elseKeyword)) {
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
	scanner.expect(OtScanner::Token::returnKeyword);

	// get return value
	if (expression(bytecode)) {
		bytecode->method(dereferenceID, 0);
	}

	// cleanup stack (since we're doing a jump over all end of blocks)
	size_t locals = 0;

	for (auto i = scopeStack.rbegin(); i->type == Scope::Type::block && i != scopeStack.rend(); i++) {
		locals += i->locals.size();
	}

	// no cleanup is required if we don't have any locals
	if (locals) {
		// move up result past the locals and shorten stack
		bytecode->move(locals);
	}

	// the return value should now be top of the stack
	bytecode->exit();
	scanner.expect(OtScanner::Token::colon);
}


//
//	OtCompiler::throwStatement
//

void OtCompiler::throwStatement(OtByteCode bytecode) {
	scanner.expect(OtScanner::Token::throwKeyword);
	bytecode->push(OtThrow::create());

	if (expression(bytecode)) {
		bytecode->method(dereferenceID, 0);
	}

	bytecode->method(callID, 1);
	scanner.expect(OtScanner::Token::colon);
}


//
//	OtCompiler::tryStatement
//

void OtCompiler::tryStatement(OtByteCode bytecode) {
	// handle "try" block
	scanner.expect(OtScanner::Token::tryKeyword);
	size_t offset1 = bytecode->pushTry();
	block(bytecode);
	bytecode->popTry();

	// jump around "catch" block
	size_t offset2 = bytecode->jump(0);
	bytecode->patchJump(offset1);

	// handle catch block
	scanner.expect(OtScanner::Token::catchKeyword);
	scanner.expect(OtScanner::Token::identifier, false);
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
	scanner.expect(OtScanner::Token::whileKeyword);
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
		case OtScanner::Token::leftBrace:
			block(bytecode);
			break;

		case OtScanner::Token::varKeyword:
			variableDeclaration(bytecode);
			break;

		case OtScanner::Token::classKeyword:
			classDeclaration(bytecode);
			break;

		case OtScanner::Token::functionKeyword:
			functionDeclaration(bytecode);
			break;

		case OtScanner::Token::doKeyword:
			doStatement(bytecode);
			break;

		case OtScanner::Token::forKeyword:
			forStatement(bytecode);
			break;

		case OtScanner::Token::ifKeyword:
			ifStatement(bytecode);
			break;

		case OtScanner::Token::returnKeyword:
			returnStatement(bytecode);
			break;

		case OtScanner::Token::throwKeyword:
			throwStatement(bytecode);
			break;

		case OtScanner::Token::tryKeyword:
			tryStatement(bytecode);
			break;

		case OtScanner::Token::whileKeyword:
			whileStatement(bytecode);
			break;

		default:
			expression(bytecode);
			bytecode->pop();
			scanner.expect(OtScanner::Token::colon);
			break;
	}

	// register statement
	bytecode->addStatement(
		sourceStart, scanner.getTokenStart(),
		opcodeStart, bytecode->size());
}
