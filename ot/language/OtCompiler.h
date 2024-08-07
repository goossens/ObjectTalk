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
#include "OtModule.h"
#include "OtOptimizer.h"
#include "OtScanner.h"
#include "OtSource.h"
#include "OtVM.h"


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

	// set debug mode
	inline static void setDebug(bool d) { debug = d; }

private:
	// push a new scope onto the scope stack
	void pushObjectScope(OtObject object);
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
			undefinedScope,
			objectScope,
			functionScope,
			blockScope
		} Type;

		// constructors
		Scope(Type t, OtObject o) : type(t), object(o) {}
		Scope(Type t) : type(t), stackFrameOffset(0) {}
		Scope(Type t, size_t sfo) : type(t), stackFrameOffset(sfo) {}

		// scope details
		Type type = undefinedScope;
		OtObject object = nullptr;
		size_t stackFrameOffset = 0;
		std::unordered_map<std::string, size_t> locals;
		std::unordered_map<std::string, OtStackItem> captures;
	};

	std::vector<Scope> scopeStack;

	// class tracker
	std::vector<OtClass> classStack;

	// debugging support
	static bool debug;
};
