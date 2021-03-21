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

#include "source.h"
#include "code.h"
#include "scanner.h"


//
//	OtCompiler
//

class OtCompiler {
public:
	// compile source code into microcode
	OtCode compile(OtSource source);

private:
	// compile function
	void function(OtCode code);

	// compile primary expression
	bool primary(OtCode code);

	// compile postfix expressions
	bool postfix(OtCode code);

	// compile prefix expressions
	bool prefix(OtCode code);

	// compile multiplication/division/power/modulo expressions
	bool multiplications(OtCode code);

	// compile addition/subtraction expressions
	bool additions(OtCode code);

	// compile shift expressions
	bool shift(OtCode code);

	// compile relationship/membership expressions
	bool relation(OtCode code);

	// compile (not) equal expressions
	bool equal(OtCode code);

	// compile bitwise "and" expression
	bool bitwiseAnd(OtCode code);

	// compile bitwise "xor" expression
	bool bitwiseXor(OtCode code);

	// compile bitwise "or" expression
	bool bitwiseOr(OtCode code);

	// compile "and" expression
	bool andExpression(OtCode code);

	// compile "or" expression
	bool orExpression(OtCode code);

	// compile a conditional expression
	bool conditional(OtCode code);

	// compile an expression
	bool expression(OtCode code);

	// compile a list of expressions
	size_t expressions(OtCode code);

	// compile a block of statements
	void block(OtCode code);

	// compile a class definition
	void classDeclaration(OtCode code);

	// compile function definition
	void functionDeclaration(OtCode code);

	// compile a do statement
	void doStatement(OtCode code);

	// compile a for statement
	void forStatement(OtCode code);

	// compile an if statement
	void ifStatement(OtCode code);

	// compile a return statement
	void returnStatement(OtCode code);

	// compile a switch statement
	void switchStatement(OtCode code);

	// compile a throw statement
	void throwStatement(OtCode code);

	// compile a try/catch statement
	void tryStatement(OtCode code);

	// compile a while statement
	void whileStatement(OtCode code);

	// compile a single statement
	void statement(OtCode code);

private:
	// source code we're compiling
	OtSource source;

	// scanner for compilation
	OtScanner scanner;
};
