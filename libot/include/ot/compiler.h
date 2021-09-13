//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>
#include <unordered_map>

#include "source.h"
#include "scanner.h"
#include "bytecode.h"
#include "module.h"
#include "stack.h"
#include "vm.h"


//
//	OtCompiler
//

class OtCompiler {
public:
	// compile module into bytecode
	OtByteCode compileModule(OtSource source, OtModule module);

	// compile expression into bytecode
	OtByteCode compileExpression(OtSource source);

private:
	// push a new scope onto the scope stack
	void pushObjectScope(OtObject object);
	void pushClassScope(const std::string& className);
	void pushFunctionScope();
	void pushBlockScope();

	// pop the last scope from the scope stack
	void popScope();

	// declare a new captured variable
	void declareCapture(const std::string& name, OtStackItem item);

	// declare a new variable in the current scope
	void declareVariable(OtByteCode bytecode, const std::string& name, bool declareVariable=false);

	// resolve variable name by pushing a reference to the stack
	void resolveVariable(OtByteCode bytecode, const std::string& name);

	// assign top stack value to named variable
	void assignVariable(OtByteCode bytecode, const std::string& name);

	// compile function
	void function(OtByteCode bytecode);

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

	// compile a switch statement
	void switchStatement(OtByteCode bytecode);

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

	// scope tracker
	typedef enum {
		UNDEFINED_SCOPE,
		OBJECT_SCOPE,
		CLASS_SCOPE,
		FUNCTION_SCOPE,
		BLOCK_SCOPE
	} OtScopeType;

	class OtScope {
	public:
		// constructors
		OtScope(OtScopeType t, OtObject o) : type(t), object(o) {}
		OtScope(OtScopeType t, const std::string& cn) : type(t), className(cn) {}
		OtScope(OtScopeType t) : type(t), stackFrameOffset(0) {}
		OtScope(OtScopeType t, size_t sfo) : type(t), stackFrameOffset(sfo) {}

		// scope details
		OtScopeType type = UNDEFINED_SCOPE;
		OtObject object = nullptr;
		std::string className;
		size_t stackFrameOffset = 0;
		std::unordered_map<std::string, size_t> locals;
		std::unordered_map<std::string, OtStackItem> captures;
	};

	std::vector<OtScope> scopeStack;
};
