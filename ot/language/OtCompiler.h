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

	// pop the last scope from the scope stack
	void popScope();

	// declare a new captured variable
	void declareCapture(const std::string& name, OtStackItem item);

	// declare a new variable in the current scope
	void declareVariable(const std::string& name, bool declareVariable=false);

	// resolve variable name by pushing a reference to the stack
	void resolveVariable(const std::string& name, bool processSymbol=true);

	// assign top stack value to named variable
	void assignVariable(const std::string& name);

	// compile function
	void function(OtByteCode bytecode, const std::string& name);

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
		typedef enum {
			objectScope,
			functionScope,
			blockScope
		} Type;

		// constructors
		Scope(OtByteCode b, OtObject o) : type(objectScope), bytecode(b), object(o) {}
		Scope(OtByteCode b) : type(functionScope), bytecode(b), stackFrameOffset(0) {}
		Scope(OtByteCode b, size_t sfo) : type(blockScope), bytecode(b), stackFrameOffset(sfo) {}

		// scope details
		Type type;
		OtByteCode bytecode;
		OtObject object = nullptr;
		size_t stackFrameOffset = 0;
		std::unordered_map<std::string, size_t> locals;
		std::unordered_map<std::string, OtStackItem> captures;
		std::vector<OtSymbol> symbols;
		std::unordered_map<std::string, size_t> symbolIndex;
	};

	std::vector<Scope> scopeStack;

	// class tracker
	std::vector<OtClass> classStack;

	// internal method identifiers used by compiler
	size_t addID = OtIdentifier::create("__add__");
	size_t assignID = OtIdentifier::create("__assign__");
	size_t bitwiseAndID = OtIdentifier::create("__band__");
	size_t bitwiseNotID = OtIdentifier::create("__bnot__");
	size_t bitwiseOrID = OtIdentifier::create("__bor__");
	size_t bitwiseXorID = OtIdentifier::create("__bxor__");
	size_t callID = OtIdentifier::create("__call__");
	size_t captureID = OtIdentifier::create("__capture__");
	size_t containsID = OtIdentifier::create("__contains__");
	size_t decrementID = OtIdentifier::create("__dec__");
	size_t dereferenceID = OtIdentifier::create("__deref__");
	size_t divideID = OtIdentifier::create("__div__");
	size_t endID = OtIdentifier::create("__end__");
	size_t equalID = OtIdentifier::create("__eq__");
	size_t greaterEqualID = OtIdentifier::create("__ge__");
	size_t greaterThenID = OtIdentifier::create("__gt__");
	size_t incrementID = OtIdentifier::create("__inc__");
	size_t indexID = OtIdentifier::create("__index__");
	size_t initID = OtIdentifier::create("__init__");
	size_t iteratorID = OtIdentifier::create("__iter__");
	size_t leftShiftID = OtIdentifier::create("__lshift__");
	size_t lessEqualID = OtIdentifier::create("__le__");
	size_t lessThenID = OtIdentifier::create("__lt__");
	size_t logicalAndID = OtIdentifier::create("__and__");
	size_t logicalNotID = OtIdentifier::create("__not__");
	size_t logicalOrID = OtIdentifier::create("__or__");
	size_t moduloID = OtIdentifier::create("__mod__");
	size_t multiplyID = OtIdentifier::create("__mul__");
	size_t negateID = OtIdentifier::create("__neg__");
	size_t nextID = OtIdentifier::create("__next__");
	size_t notEqualID = OtIdentifier::create("__ne__");
	size_t parentID = OtIdentifier::create("__parent__");
	size_t plusID = OtIdentifier::create("__plus__");
	size_t powerID = OtIdentifier::create("__power__");
	size_t rightShiftID = OtIdentifier::create("__rshift__");
	size_t subtractID = OtIdentifier::create("__sub__");
};
