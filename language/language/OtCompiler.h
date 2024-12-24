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
#include <utility>

#include "OtByteCode.h"
#include "OtClass.h"
#include "OtIdentifier.h"
#include "OtModule.h"
#include "OtOptimizer.h"
#include "OtScanner.h"
#include "OtSource.h"
#include "OtStack.h"
#include "OtSymbol.h"


//
//	OtCompiler
//

class OtCompiler {
public:
	// compile ObjectTalk script into bytecode
	OtByteCode compileFile(const std::string& path);
	OtByteCode compileText(const std::string& text);
	OtByteCode compileSource(OtSource source, OtObject object);

	// compile expression into bytecode
	OtByteCode compileExpression(OtSource source);

private:
	// push a new scope onto the scope stack
	void pushObjectScope(OtByteCode bytecode, OtObject object);
	void pushFunctionScope(OtByteCode bytecode);
	void pushBlockScope(OtByteCode bytecode);

	// close scope by finalizing local symbol "visibility"
	void closeScope();

	// pop the last scope from the scope stack
	void popScope();

	// declare a new captured variable
	void declareCapture(OtID id, std::pair<size_t, size_t> item);

	// declare a new variable in the current scope
	void declareVariable(OtID id, bool declareVariable=false);

	// resolve variable name by pushing a reference to the stack
	void resolveVariable(OtID id, bool processSymbol=true);

	// assign top stack value to named variable
	void assignVariable(OtID id);

	// compile function
	void function(OtByteCode bytecode, OtID id);

	// compile superclass reference
	void super(OtByteCode bytecode);

	// compile primary expression
	bool primary(OtByteCode bytecode);

	// compile postfix expressions
	bool postfix(OtByteCode bytecode);

	// compile prefix expressions
	bool prefix(OtByteCode bytecode);

	// compile multiplication/division/power/modulo expressions
	bool multiplications(OtByteCode bytecode);

	// compile addition/subtraction expressions
	bool additions(OtByteCode bytecode);

	// compile shift expressions
	bool shift(OtByteCode bytecode);

	// compile relationship/membership expressions
	bool relation(OtByteCode bytecode);

	// compile (not) equal expressions
	bool equal(OtByteCode bytecode);

	// compile bitwise "and" expression
	bool bitwiseAnd(OtByteCode bytecode);

	// compile bitwise "xor" expression
	bool bitwiseXor(OtByteCode bytecode);

	// compile bitwise "or" expression
	bool bitwiseOr(OtByteCode bytecode);

	// compile "and" expression
	bool andExpression(OtByteCode bytecode);

	// compile "or" expression
	bool orExpression(OtByteCode bytecode);

	// compile a conditional expression
	bool conditional(OtByteCode bytecode);

	// compile an expression
	bool expression(OtByteCode bytecode);

	// compile a list of expressions
	size_t expressions(OtByteCode bytecode);

	// compile a block of statements
	void block(OtByteCode bytecode);

	// compile a variable declatation
	void variableDeclaration(OtByteCode bytecode);

	// compile a class declatation
	void classDeclaration(OtByteCode bytecode);

	// compile function declatation
	void functionDeclaration(OtByteCode bytecode);

	// compile a do statement
	void doStatement(OtByteCode bytecode);

	// compile a for statement
	void forStatement(OtByteCode bytecode);

	// compile an if statement
	void ifStatement(OtByteCode bytecode);

	// compile a return statement
	void returnStatement(OtByteCode bytecode);

	// compile a throw statement
	void throwStatement(OtByteCode bytecode);

	// compile a try/catch statement
	void tryStatement(OtByteCode bytecode);

	// compile a while statement
	void whileStatement(OtByteCode bytecode);

	// compile a single statement
	void statement(OtByteCode bytecode);

private:
	// source code we're compiling
	OtSource source;

	// scanner for compilation
	OtScanner scanner;

	// code optimizer
	OtOptimizer optimizer;

	// scope tracker
	class Scope {
	public:
		// scope types
		enum class Type {
			object,
			function,
			block
		};

		// constructors
		Scope(OtByteCode b, OtObject o) : type(Type::object), bytecode(b), object(o) {}
		Scope(OtByteCode b) : type(Type::function), bytecode(b) {}
		Scope(OtByteCode b, size_t sfo) : type(Type::block), bytecode(b), stackFrameOffset(sfo) {}

		// scope details
		Type type;
		OtByteCode bytecode;
		OtObject object = nullptr;
		size_t stackFrameOffset = 0;
		std::unordered_map<OtID, size_t> locals;
		std::unordered_map<OtID, std::pair<size_t, size_t>> captures;
		std::vector<OtSymbol> symbols;
		std::unordered_map<OtID, size_t> symbolIndex;
		bool closed = false;
	};

	std::vector<Scope> scopeStack;

	// class tracker
	std::vector<OtClass> classStack;

	// current statement start
	size_t statementStart;

	// internal method identifiers used by compiler
	OtID addID = OtIdentifier::create("__add__");
	OtID anonymousID = OtIdentifier::create("__anonymous__");
	OtID assignID = OtIdentifier::create("__assign__");
	OtID bitwiseAndID = OtIdentifier::create("__band__");
	OtID bitwiseNotID = OtIdentifier::create("__bnot__");
	OtID bitwiseOrID = OtIdentifier::create("__bor__");
	OtID bitwiseXorID = OtIdentifier::create("__bxor__");
	OtID callID = OtIdentifier::create("__call__");
	OtID captureID = OtIdentifier::create("__capture__");
	OtID containsID = OtIdentifier::create("__contains__");
	OtID decrementID = OtIdentifier::create("__dec__");
	OtID dereferenceID = OtIdentifier::create("__deref__");
	OtID divideID = OtIdentifier::create("__div__");
	OtID endID = OtIdentifier::create("__end__");
	OtID equalID = OtIdentifier::create("__eq__");
	OtID expressionID = OtIdentifier::create("__expression__");
	OtID greaterEqualID = OtIdentifier::create("__ge__");
	OtID greaterThenID = OtIdentifier::create("__gt__");
	OtID incrementID = OtIdentifier::create("__inc__");
	OtID indexID = OtIdentifier::create("__index__");
	OtID initID = OtIdentifier::create("__init__");
	OtID iteratorID = OtIdentifier::create("__iter__");
	OtID leftShiftID = OtIdentifier::create("__lshift__");
	OtID lessEqualID = OtIdentifier::create("__le__");
	OtID lessThenID = OtIdentifier::create("__lt__");
	OtID logicalAndID = OtIdentifier::create("__and__");
	OtID logicalNotID = OtIdentifier::create("__not__");
	OtID logicalOrID = OtIdentifier::create("__or__");
	OtID moduloID = OtIdentifier::create("__mod__");
	OtID multiplyID = OtIdentifier::create("__mul__");
	OtID negateID = OtIdentifier::create("__neg__");
	OtID nextID = OtIdentifier::create("__next__");
	OtID notEqualID = OtIdentifier::create("__ne__");
	OtID parentID = OtIdentifier::create("__parent__");
	OtID plusID = OtIdentifier::create("__plus__");
	OtID powerID = OtIdentifier::create("__power__");
	OtID rightShiftID = OtIdentifier::create("__rshift__");
	OtID subtractID = OtIdentifier::create("__sub__");

	// internal method identifiers used by compiler
	OtID blankID = OtIdentifier::create("");
	OtID arrayID = OtIdentifier::create("Array");
	OtID dictID = OtIdentifier::create("Dict");
};
