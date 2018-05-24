//	ObjectTalk Scripting Language
//	Copyright 1993-2018 Johan A. Goossens
//
//	Licensed under the Apache License, Version 2.0 (the "License");
//	you may not use this file except in compliance with the License.
//	You may obtain a copy of the License at
//
//		http://www.apache.org/licenses/LICENSE-2.0
//
//	Unless required by applicable law or agreed to in writing, software
//	distributed under the License is distributed on an "AS IS" BASIS,
//	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//	See the License for the specific language governing permissions and
//	limitations under the License.


#pragma once


//
//  Include files
//

#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "OtClasses.h"
#include "OtException.h"
#include "OtScanner.h"


//
//  OtObjectTalk
//

class OtObjectTalk
{
public:
	// constructor
	OtObjectTalk() {}

	// compile text into microcode
	OtCode compile(const std::string& text)
	{
		// load scanner
		scanner.loadText(text);

		// setup code
		OtCode code = OtCodeCreate();

		// process all statements
		if (scanner.matchToken(OtScanner::OT_SCANNER_EOS_TOKEN))
			code->push(nullptr);

		else
		{
			statement(code);

			while (!scanner.matchToken(OtScanner::OT_SCANNER_EOS_TOKEN))
			{
				code->pop();
				statement(code);
			}
		}

		// return compiled code
		return code;
	}

	// create a default object talk context
	OtValue createDefaultContext()
	{
		// create global context
		OtValue context = OtValueCreate();
		context->set("global", context);

		// add default constants
		context->set("true", OtValueCreate(true));
		context->set("false", OtValueCreate(false));
		context->set("null", nullptr);

		// add default functions
		context->set("assert", OtFunctionCreateRaw([] (OtValue context, size_t c, OtValue* p)->OtValue
		{
			if (c != 1)
				OT_EXCEPT("Function expects 1 parameter, %d given", c);

			OtObjectTalk ot;
			std::string assertion = p[0]->operator std::string();
			OtValue result = ot.processText(assertion, context);
			
			if (!result->operator bool())
				OT_EXCEPT("Assertion [%s] failed", assertion.c_str());

			return nullptr;
		}));

		context->set("print", OtFunctionCreateRaw([] (OtValue, size_t c, OtValue* p)->OtValue
		{
			for (size_t i = 0; i < c; i++)
				std::cout << (std::string) *p[i];

			std::cout << std::endl;
			return nullptr;
		}));

		// add default classes
		context->set("Object", OtObjectClassCreate());

		context->set("Internal", OtInternalClassCreate());
		context->set("Class", OtClassClassCreate());
		context->set("MemberReference", OtMemberReferenceClassCreate());
		context->set("ArrayReference", OtArrayReferenceClassCreate());
		context->set("DictReference", OtDictReferenceClassCreate());

		context->set("Primitive", OtPrimitiveClassCreate());
		context->set("Boolean", OtBooleanClassCreate());
		context->set("Integer", OtIntegerClassCreate());
		context->set("Real", OtRealClassCreate());
		context->set("String", OtStringClassCreate());
		context->set("Function", OtFunctionClassCreate());

		context->set("Collection", OtCollectionClassCreate());
		context->set("Array", OtArrayClassCreate());
		context->set("Dict", OtDictClassCreate());

		// return default context
		return context;
	}

	// compile and run ObjectTalk text
	OtValue processText(const std::string& text, OtValue context=nullptr)
	{
		// compile the code
		OtCode code = compile(text);

		// create default context if required
		if (!context)
			context = createDefaultContext();

		// execute code and return result
		return code->run(context);
	}

	// compile and run an ObjectTalk file
	OtValue processFile(const std::string& filename, OtValue context=nullptr)
	{
		// get text from file and process
		std::ifstream stream(filename);
		std::stringstream buffer;
		buffer << stream.rdbuf();
		return processText(buffer.str(), context);
	}

private:
	// compile anonymous function
	void function(OtCode code)
	{
		// local variables
		size_t count;
		std::vector<std::string> names;

		// skip function and opening parenthesis
		scanner.expect(OtScanner::OT_SCANNER_FUNCTION_TOKEN);
		scanner.expect(OtScanner::OT_SCANNER_LPAREN_TOKEN);

		// handle ellipsis for variable argument count
		if (scanner.matchToken(OtScanner::OT_SCANNER_ELLIPSIS_TOKEN))
		{
			scanner.advance();
			scanner.expect(OtScanner::OT_SCANNER_RPAREN_TOKEN);
			count = SIZE_MAX;
		}

		else
		{
			// get parameter names
			while (!scanner.matchToken(OtScanner::OT_SCANNER_RPAREN_TOKEN) && !scanner.matchToken(OtScanner::OT_SCANNER_EOS_TOKEN))
			{
				scanner.expect(OtScanner::OT_SCANNER_IDENTIFIER_TOKEN, false);
				names.push_back(scanner.getText());
				scanner.advance();

				if (scanner.matchToken(OtScanner::OT_SCANNER_COMMA_TOKEN))
					scanner.advance();
			}

			scanner.expect(OtScanner::OT_SCANNER_RPAREN_TOKEN);
			count = names.size();
		}

		// get function level code
		OtCode body = OtCodeCreate();
		statement(body);
		code->push(OtValueCreate(count, body, names));
	}

	// compile primary expression
	bool primary(OtCode code)
	{
		bool reference;
		size_t count = 0;

		switch (scanner.getToken())
		{
			case OtScanner::OT_SCANNER_LPAREN_TOKEN:
				// handle sub-expression
				scanner.advance();
				reference = expression(code);
				scanner.expect(OtScanner::OT_SCANNER_RPAREN_TOKEN);
				break;

			case OtScanner::OT_SCANNER_INTEGER_TOKEN:
				// handle integer constants
				code->push(OtValueCreate(scanner.getInteger()));
				scanner.advance();
				reference = false;
				break;

			case OtScanner::OT_SCANNER_REAL_TOKEN:
				// handle real constants
				code->push(OtValueCreate(scanner.getReal()));
				scanner.advance();
				reference = false;
				break;

			case OtScanner::OT_SCANNER_STRING_TOKEN:
				// handle string constants
				code->push(OtValueCreate(scanner.getString()));
				scanner.advance();
				reference = false;
				break;

			case OtScanner::OT_SCANNER_FUNCTION_TOKEN:
				// handle function definition
				function(code);
				reference = false;
				break;

			case OtScanner::OT_SCANNER_IDENTIFIER_TOKEN:
				// handle named reference
				code->push(OtContextReferenceCreate(scanner.getText()));
				scanner.advance();
				reference = true;
				break;

			case OtScanner::OT_SCANNER_LBRACKET_TOKEN:
				// handle array constant
				scanner.advance();
				code->push(OtArrayCreate());
				code->method("__init__", expressions(code));
				scanner.expect(OtScanner::OT_SCANNER_RBRACKET_TOKEN);
				reference = false;
				break;

			case OtScanner::OT_SCANNER_LBRACE_TOKEN:
				// handle dictionary constant
				scanner.advance();
				code->push(OtDictCreate());

				while (scanner.getToken() != OtScanner::OT_SCANNER_RBRACE_TOKEN && scanner.getToken() != OtScanner::OT_SCANNER_EOS_TOKEN)
				{
					scanner.expect(OtScanner::OT_SCANNER_IDENTIFIER_TOKEN, false);
					code->push(OtValueCreate(scanner.getText()));
					scanner.advance();
					scanner.expect(OtScanner::OT_SCANNER_COLON_TOKEN);

					if (expression(code))
						code->method("__deref__", 0);

					count += 2;

					if (scanner.getToken() == OtScanner::OT_SCANNER_COMMA_TOKEN)
						scanner.advance();
				}

				scanner.expect(OtScanner::OT_SCANNER_RBRACE_TOKEN);
				code->method("__init__", count);
				reference = false;
				break;

			default:
				reference = false;
				scanner.error("Primary expected");
		}

		return reference;
	}

	// compile postfix expressions
	bool postfix(OtCode code)
	{
		// parse left side
		bool reference = primary(code);
		OtToken token = scanner.getToken();

		// process postfix
		while (token == OtScanner::OT_SCANNER_LBRACKET_TOKEN ||
			   token == OtScanner::OT_SCANNER_LPAREN_TOKEN ||
			   token == OtScanner::OT_SCANNER_PERIOD_TOKEN ||
			   token == OtScanner::OT_SCANNER_INCREMENT_TOKEN ||
			   token == OtScanner::OT_SCANNER_DECREMENT_TOKEN)
		{
			scanner.advance();
			std::string member;
			size_t count;

			// generate code
			switch (token)
			{
				case OtScanner::OT_SCANNER_LBRACKET_TOKEN:
					// index object
					if (reference)
						code->method("__deref__", 0);

					if (expression(code))
						code->method("__deref__", 0);

					scanner.expect(OtScanner::OT_SCANNER_RBRACKET_TOKEN);
					code->method("__index__", 1);
					reference = true;
					break;

				case OtScanner::OT_SCANNER_LPAREN_TOKEN:
					// call object
					if (reference)
						code->method("__deref__", 0);

					count = scanner.matchToken(OtScanner::OT_SCANNER_RPAREN_TOKEN) ? 0 : expressions(code);
					scanner.expect(OtScanner::OT_SCANNER_RPAREN_TOKEN);
					code->method("__call__", count);
					reference = false;
					break;

				case OtScanner::OT_SCANNER_PERIOD_TOKEN:
					// member access
					if (reference)
						code->method("__deref__", 0);

					scanner.expect(OtScanner::OT_SCANNER_IDENTIFIER_TOKEN, false);
					code->push(OtValueCreate(scanner.getText()));
					scanner.advance();
					code->method("__member__", 1);
					reference = true;

					break;

				case OtScanner::OT_SCANNER_INCREMENT_TOKEN:
					if (!reference)
						scanner.error("Lvalue required for '++'");

					code->dup();
					code->method("__deref__", 0);
					code->swap();
					code->dup();
					code->method("__deref__", 0);
					code->method("__inc__", 0);
					code->method("__assign__", 1);
					code->pop();
					reference = false;
					break;

				case OtScanner::OT_SCANNER_DECREMENT_TOKEN:
					if (!reference)
						scanner.error("Lvalue required for '--'");

					code->dup();
					code->method("__deref__", 0);
					code->swap();
					code->dup();
					code->method("__deref__", 0);
					code->method("__dec__", 0);
					code->method("__assign__", 1);
					code->pop();
					reference = false;
					break;
			}

			token = scanner.getToken();
		}

		return reference;
	}

	// compile prefix expressions
	bool prefix(OtCode code)
	{
		OtToken token = scanner.getToken();

		// process prefix
		if (token == OtScanner::OT_SCANNER_SUBTRACT_TOKEN ||
			token == OtScanner::OT_SCANNER_NOT_TOKEN ||
			token == OtScanner::OT_SCANNER_BITWISE_NOT_TOKEN ||
			token == OtScanner::OT_SCANNER_INCREMENT_TOKEN ||
			token == OtScanner::OT_SCANNER_DECREMENT_TOKEN)
		{
			scanner.advance();
			bool reference = postfix(code);

			// generate code
			switch (token)
			{
				case OtScanner::OT_SCANNER_SUBTRACT_TOKEN:
					if (reference)
						code->method("__deref__", 0);

					code->method("__neg__", 0);
					break;

				case OtScanner::OT_SCANNER_NOT_TOKEN:
					if (reference)
						code->method("__deref__", 0);

					code->method("__not__", 0);
					break;

				case OtScanner::OT_SCANNER_BITWISE_NOT_TOKEN:
					if (reference)
						code->method("__deref__", 0);

					code->method("__bnot__", 0);
					break;

				case OtScanner::OT_SCANNER_INCREMENT_TOKEN:
					if (!reference)
						scanner.error("Lvalue required for '++'");

					code->dup();
					code->method("__deref__", 0);
					code->method("__inc__", 0);
					code->method("__assign__", 0);
					break;

				case OtScanner::OT_SCANNER_DECREMENT_TOKEN:
					if (!reference)
						scanner.error("Lvalue required for '--'");

					code->dup();
					code->method("__deref__", 0);
					code->method("__dec__", 0);
					code->method("__assign__", 0);
					break;
			}

			return false;
		}

		else
			return postfix(code);
	}

	// compile multiplication/division/power/modulo expressions
	bool multiplications(OtCode code)
	{
		// parse left side
		bool reference = prefix(code);
		OtToken token = scanner.getToken();

		while (token == OtScanner::OT_SCANNER_MULTIPLY_TOKEN ||
			   token == OtScanner::OT_SCANNER_DIVIDE_TOKEN ||
			   token == OtScanner::OT_SCANNER_POWER_TOKEN ||
			   token == OtScanner::OT_SCANNER_MODULO_TOKEN)
		{
			// deref if required
			scanner.advance();

			if (reference)
				code->method("__deref__", 0);

			// parse right side
			if (prefix(code))
				code->method("__deref__", 0);

			// generate code
			switch (token)
			{
				case OtScanner::OT_SCANNER_MULTIPLY_TOKEN:
					code->method("__mul__", 1);
					break;

				case OtScanner::OT_SCANNER_DIVIDE_TOKEN:
					code->method("__div__", 1);
					break;

				case OtScanner::OT_SCANNER_POWER_TOKEN:
					code->method("__power__", 1);
					break;

				case OtScanner::OT_SCANNER_MODULO_TOKEN:
					code->method("__mod__", 1);
					break;
			}

			token = scanner.getToken();
			reference = false;
		}

		return reference;
	}

	// compile addition/subtraction expressions
	bool additions(OtCode code)
	{
		// parse left side
		bool reference = multiplications(code);
		OtToken token = scanner.getToken();

		while (token == OtScanner::OT_SCANNER_ADD_TOKEN || token == OtScanner::OT_SCANNER_SUBTRACT_TOKEN)
		{
			// deref if required
			scanner.advance();

			if (reference)
				code->method("__deref__", 0);

			// parse right side
			if (multiplications(code))
				code->method("__deref__", 0);

			// generate code
			if (token == OtScanner::OT_SCANNER_ADD_TOKEN)
				code->method("__add__", 1);

			else
				code->method("__sub__", 1);

			token = scanner.getToken();
			reference = false;
		}

		return reference;
	}

	// compile shift expressions
	bool shift(OtCode code)
	{
		// parse left side
		bool reference = additions(code);
		OtToken token = scanner.getToken();

		while (token == OtScanner::OT_SCANNER_SHIFT_LEFT_TOKEN || token == OtScanner::OT_SCANNER_SHIFT_RIGHT_TOKEN)
		{
			// deref if required
			scanner.advance();

			if (reference)
				code->method("__deref__", 0);

			// parse right side
			if (additions(code))
				code->method("__deref__", 0);

			// generate code
			if (token == OtScanner::OT_SCANNER_SHIFT_LEFT_TOKEN)
				code->method("__lshift__", 1);

			else
				code->method("__rshift__", 1);

			token = scanner.getToken();
			reference = false;
		}

		return reference;
	}

	// compile relationship expressions
	bool relation(OtCode code)
	{
		// parse left side
		bool reference = shift(code);
		OtToken token = scanner.getToken();

		while (token == OtScanner::OT_SCANNER_LESS_TOKEN ||
			   token == OtScanner::OT_SCANNER_LESS_EQUAL_TOKEN ||
			   token == OtScanner::OT_SCANNER_GREATER_TOKEN ||
			   token == OtScanner::OT_SCANNER_GREATER_EQUAL_TOKEN)
		{
			// deref if required
			scanner.advance();

			if (reference)
				code->method("__deref__", 0);

			// parse right side
			if (shift(code))
				code->method("__deref__", 0);

			// generate code
			switch (token)
			{
				case OtScanner::OT_SCANNER_LESS_TOKEN:
					code->method("__lt__", 1);
					break;

				case OtScanner::OT_SCANNER_LESS_EQUAL_TOKEN:
					code->method("__le__", 1);
					break;

				case OtScanner::OT_SCANNER_GREATER_TOKEN:
					code->method("__gt__", 1);
					break;

				case OtScanner::OT_SCANNER_GREATER_EQUAL_TOKEN:
					code->method("__ge__", 1);
					break;
			}

			token = scanner.getToken();
			reference = false;
		}

		return reference;
	}

	// compile (not) equal expressions
	bool equal(OtCode code)
	{
		// parse left side
		bool reference = relation(code);
		OtToken token = scanner.getToken();

		while (token == OtScanner::OT_SCANNER_EQUAL_TOKEN || token == OtScanner::OT_SCANNER_NOT_EQUAL_TOKEN)
		{
			// deref if required
			scanner.advance();

			if (reference)
				code->method("__deref__", 0);

			// parse right side
			if (relation(code))
				code->method("__deref__", 0);

			// generate code
			if (token == OtScanner::OT_SCANNER_EQUAL_TOKEN)
				code->method("__eq__", 1);

			else
				code->method("__ne__", 1);

			token = scanner.getToken();
			reference = false;
		}

		return reference;
	}

	// compile bitwise "and" expression
	bool bitwiseAnd(OtCode code)
	{
		// parse left side
		bool reference = equal(code);

		// handle all "&" operators
		if (scanner.matchToken(OtScanner::OT_SCANNER_BITWISE_AND_TOKEN))
		{
			// deref if required
			scanner.advance();

			if (reference)
				code->method("__deref__", 0);

			// parse right side
			if (equal(code))
				code->method("__deref__", 0);

			// generate code
			code->method("__bend__", 1);
			reference = false;
		}

		return reference;
	}

	// compile bitwise "xor" expression
	bool bitwiseXor(OtCode code)
	{
		// parse left side
		bool reference = bitwiseAnd(code);

		// handle all "&" operators
		if (scanner.matchToken(OtScanner::OT_SCANNER_BITWISE_XOR_TOKEN))
		{
			// deref if required
			scanner.advance();

			if (reference)
				code->method("__deref__", 0);

			// parse right side
			if (bitwiseAnd(code))
				code->method("__deref__", 0);

			// generate code
			code->method("__bxor__", 1);
			reference = false;
		}

		return reference;
	}

	// compile bitwise "or" expression
	bool bitwiseOr(OtCode code)
	{
		// parse left side
		bool reference = bitwiseXor(code);

		// handle all "|" operators
		if (scanner.matchToken(OtScanner::OT_SCANNER_BITWISE_OR_TOKEN))
		{
			// deref if required
			scanner.advance();

			if (reference)
				code->method("__deref__", 0);

			// parse right side
			if (bitwiseXor(code))
				code->method("__deref__", 0);

			// generate code
			code->method("__bor__", 1);
			reference = false;
		}

		return reference;
	}

	// compile "and" expression
	bool andExpression(OtCode code)
	{
		// parse left side
		bool reference = bitwiseOr(code);

		// handle all "&&" operators
		while (scanner.matchToken(OtScanner::OT_SCANNER_AND_TOKEN))
		{
			// skip token
			scanner.advance();

			// deref if required
			if (reference)
				code->method("__deref__", 0);

			// parse right side
			if (bitwiseOr(code))
				code->method("__deref__", 0);

			// generate code
			code->method("__and__", 1);
			reference = false;
		}

		return reference;
	}

	// compile "or" expression
	bool orExpression(OtCode code)
	{
		// parse left side
		bool reference = andExpression(code);

		// handle all "||" operators
		while (scanner.matchToken(OtScanner::OT_SCANNER_OR_TOKEN))
		{
			// skip token
			scanner.advance();

			// deref if required
			if (reference)
				code->method("__deref__", 0);

			// parse right side
			if (andExpression(code))
				code->method("__deref__", 0);

			// generate code
			code->method("__or__", 1);
			reference = false;
		}

		return reference;
	}

	// compile a conditional expression
	bool conditional(OtCode code)
	{
		// parse left side
		bool reference = orExpression(code);

		if (scanner.matchToken(OtScanner::OT_SCANNER_QUESTION_TOKEN))
		{
			// deref if required
			scanner.advance();

			if (reference)
				code->method("__deref__", 0);

			// evaluate expression
			size_t offset1 = code->size();
			code->jumpFalse(0);

			// process "true" expression
			if (expression(code))
				code->method("__deref__", 0);

			scanner.expect(OtScanner::OT_SCANNER_COLON_TOKEN);

			// jump around "false" expression and patch first jump
			size_t offset2 = code->size();
			code->jump(0);
			code->patch(offset1);

			// process "false" expression
			if (expression(code))
				code->method("__deref__", 0);

			// patch jump around false
			code->patch(offset2);
			reference = false;
		}

		return reference;
	}

	// compile an expression
	bool expression(OtCode code)
	{
		// parse left side
		bool reference = conditional(code);
		OtToken token = scanner.getToken();

		// process all assignment operators
		while (token == OtScanner::OT_SCANNER_ASSIGNMENT_TOKEN ||
			   token == OtScanner::OT_SCANNER_MULTIPLY_ASSIGNMENT_TOKEN ||
			   token == OtScanner::OT_SCANNER_DIVIDE_ASSIGNMENT_TOKEN ||
			   token == OtScanner::OT_SCANNER_MODULO_ASSIGNMENT_TOKEN ||
			   token == OtScanner::OT_SCANNER_ADD_ASSIGNMENT_TOKEN ||
			   token == OtScanner::OT_SCANNER_SUBTRACT_ASSIGNMENT_TOKEN ||
			   token == OtScanner::OT_SCANNER_BITWISE_AND_ASSIGNMENT_TOKEN ||
			   token == OtScanner::OT_SCANNER_BITWISE_OR_ASSIGNMENT_TOKEN ||
			   token == OtScanner::OT_SCANNER_BITWISE_XOR_ASSIGNMENT_TOKEN)
		{
			// sanity check
			if (!reference)
				scanner.error("Lvalue required for assignments");

			// duplicate left side if required
			if (token != OtScanner::OT_SCANNER_ASSIGNMENT_TOKEN)
			{
				code->dup();
				code->method("__deref__", 0);
			}

			// parse right side
			scanner.advance();
			reference = conditional(code);

			// dereference if required
			if (reference)
				code->method("__deref__", 0);

			// handle different assignments
			switch (token)
			{
				case OtScanner::OT_SCANNER_MULTIPLY_ASSIGNMENT_TOKEN:
					code->method("__mul__", 1);
					break;

				case OtScanner::OT_SCANNER_DIVIDE_ASSIGNMENT_TOKEN:
					code->method("__div__", 1);
					break;

				case OtScanner::OT_SCANNER_MODULO_ASSIGNMENT_TOKEN:
					code->method("__mod__", 1);
					break;

				case OtScanner::OT_SCANNER_ADD_ASSIGNMENT_TOKEN:
					code->method("__add__", 1);
					break;

				case OtScanner::OT_SCANNER_SUBTRACT_ASSIGNMENT_TOKEN:
					code->method("__sub__", 1);
					break;

				case OtScanner::OT_SCANNER_BITWISE_AND_ASSIGNMENT_TOKEN:
					code->method("__bend__", 1);
					break;

				case OtScanner::OT_SCANNER_BITWISE_OR_ASSIGNMENT_TOKEN:
					code->method("__bor__", 1);
					break;

				case OtScanner::OT_SCANNER_BITWISE_XOR_ASSIGNMENT_TOKEN:
					code->method("__bxor__", 1);
					break;
			}

			// perform assignment
			code->method("__assign__", 1);
			reference = false;

			token = scanner.getToken();
		}

		return reference;
	}

	// compile a list of expressions
	size_t expressions(OtCode code)
	{
		// process comma seperated list of expressions
		size_t count = 1;

		if (expression(code))
			code->method("__deref__", 0);

		while (scanner.matchToken(OtScanner::OT_SCANNER_COMMA_TOKEN))
		{
			scanner.advance();

			if (expression(code))
				code->method("__deref__", 0);

			count++;
		}

		return count;
	}

	// compile a block of statements
	void block(OtCode code)
	{
		scanner.expect(OtScanner::OT_SCANNER_LBRACE_TOKEN);

		if (scanner.matchToken(OtScanner::OT_SCANNER_RBRACE_TOKEN))
			code->push(nullptr);

		else
		{
			statement(code);

			while (!scanner.matchToken(OtScanner::OT_SCANNER_RBRACE_TOKEN) && !scanner.matchToken(OtScanner::OT_SCANNER_EOS_TOKEN))
			{
				code->pop();
				statement(code);
			}
		}

		scanner.expect(OtScanner::OT_SCANNER_RBRACE_TOKEN);
	}

	// compile a class definition
	void classDeclaration(OtCode code)
	{
		// handle class name
		scanner.expect(OtScanner::OT_SCANNER_CLASS_TOKEN);
		scanner.expect(OtScanner::OT_SCANNER_IDENTIFIER_TOKEN, false);
		std::string name = scanner.getText();
		scanner.advance();

		// handle parent name
		scanner.expect(OtScanner::OT_SCANNER_COLON_TOKEN);
		scanner.expect(OtScanner::OT_SCANNER_IDENTIFIER_TOKEN, false);
		std::string parent = scanner.getText();
		scanner.advance();

		// create new class
		code->get(parent);
		code->push(OtValueCreate(name));
		code->method("subClass", 1);

		// assign class
		code->dup();
		code->dup();
		code->set(name);

		// process class content
		code->pushContext();
		statement(code);
		code->pop();
		code->popContext();
	}

	// compile function definition
	void functionDeclaration(OtCode code)
	{
		// local variables
		size_t count;
		std::vector<std::string> names;

		// get function name
		scanner.expect(OtScanner::OT_SCANNER_FUNCTION_TOKEN);
		scanner.expect(OtScanner::OT_SCANNER_IDENTIFIER_TOKEN, false);
		std::string name = scanner.getText();
		scanner.advance();
		scanner.expect(OtScanner::OT_SCANNER_LPAREN_TOKEN);

		// handle ellipsis for variable argument count
		if (scanner.matchToken(OtScanner::OT_SCANNER_ELLIPSIS_TOKEN))
		{
			scanner.advance();
			count = SIZE_MAX;
		}

		else
		{
			// get parameter names
			while (!scanner.matchToken(OtScanner::OT_SCANNER_RPAREN_TOKEN) && !scanner.matchToken(OtScanner::OT_SCANNER_EOS_TOKEN))
			{
				scanner.expect(OtScanner::OT_SCANNER_IDENTIFIER_TOKEN, false);
				names.push_back(scanner.getText());
				scanner.advance();

				if (scanner.matchToken(OtScanner::OT_SCANNER_COMMA_TOKEN))
					scanner.advance();
			}

			count = names.size();
		}

		scanner.expect(OtScanner::OT_SCANNER_RPAREN_TOKEN);

		// get function level code
		OtCode body = OtCodeCreate();
		statement(body);

		// add function to context
		code->push(OtValueCreate(count, body, names));
		code->dup();
		code->set(name);
	}

	// compile a do statement
	void doStatement(OtCode code)
	{
		scanner.expect(OtScanner::OT_SCANNER_DO_TOKEN);
		size_t offset = code->size();

		statement(code);
		code->pop();

		scanner.expect(OtScanner::OT_SCANNER_WHILE_TOKEN);

		if (expression(code))
			code->method("__deref__", 0);

		code->jumpTrue(offset);
	}

	// compile a for statement
	void forStatement(OtCode code)
	{
		scanner.expect(OtScanner::OT_SCANNER_FOR_TOKEN);
		scanner.expect(OtScanner::OT_SCANNER_LPAREN_TOKEN);

		if (!scanner.matchToken(OtScanner::OT_SCANNER_SEMICOLON_TOKEN))
			code->pop(expressions(code));

		scanner.expect(OtScanner::OT_SCANNER_SEMICOLON_TOKEN);

		size_t offset1 = code->size();

		if (expression(code))
			code->method("__deref__", 0);

		size_t offset2 = code->size();
		code->jumpFalse(0);

		scanner.expect(OtScanner::OT_SCANNER_SEMICOLON_TOKEN);

		OtCode tmp = OtCodeCreate();

		if (!scanner.matchToken(OtScanner::OT_SCANNER_RPAREN_TOKEN))
			tmp->pop(expressions(tmp));

		scanner.expect(OtScanner::OT_SCANNER_RPAREN_TOKEN);
		statement(code);
		code->pop();

		code->insert(code->end(), tmp->begin(), tmp->end());

		code->jump(offset1);
		code->patch(offset2);
	}

	// compile an if statement
	void ifStatement(OtCode code)
	{
		scanner.expect(OtScanner::OT_SCANNER_IF_TOKEN);

		if (expression(code))
			code->method("__deref__", 0);

		size_t offset1 = code->size();
		code->jumpFalse(0);

		statement(code);
		code->pop();

		if (scanner.matchToken(OtScanner::OT_SCANNER_ELSE_TOKEN))
		{
			scanner.advance();

			size_t offset2 = code->size();
			code->jump(0);

			code->patch(offset1);
			statement(code);
			code->pop();

			code->patch(offset2);
		}

		else
			code->patch(offset1);
	}

	// compile a return statement
	void returnStatement(OtCode code)
	{
		scanner.expect(OtScanner::OT_SCANNER_RETURN_TOKEN);

		if (expression(code))
			code->method("__deref__", 0);

		code->exit();
	}

	// compile a switch statement
	void switchStatement(OtCode code)
	{
		std::vector<size_t> patches;

		scanner.expect(OtScanner::OT_SCANNER_SWITCH_TOKEN);

		if (expression(code))
			code->method("__deref__", 0);

			while (scanner.matchToken(OtScanner::OT_SCANNER_CASE_TOKEN))
			{
				scanner.advance();
				code->dup();

				if (expression(code))
					code->method("__deref__", 0);

					code->method("__eq__", 1);

					size_t offset1 = code->size();
					code->jumpFalse(0);

					statement(code);
					code->pop();

					patches.push_back(code->size());
					code->jump(0);

					code->patch(offset1);
					}

		if (scanner.matchToken(OtScanner::OT_SCANNER_DEFAULT_TOKEN))
		{
			scanner.advance();
			statement(code);
			code->pop();
		}

		for (auto const& patch : patches)
			code->patch(patch);

			code->pop();
			}

	// compile a while statement
	void whileStatement(OtCode code)
	{
		scanner.expect(OtScanner::OT_SCANNER_WHILE_TOKEN);
		size_t offset1 = code->size();

		if (expression(code))
			code->method("__deref__", 0);

		size_t offset2 = code->size();
		code->jumpFalse(0);

		statement(code);
		code->pop();
		code->jump(offset1);
		code->patch(offset2);
	}

	// compile a single statement
	void statement(OtCode code)
	{
		// process statement
		switch (scanner.getToken())
		{
			case OtScanner::OT_SCANNER_LBRACE_TOKEN:
				block(code);
				break;

			case OtScanner::OT_SCANNER_CLASS_TOKEN:
				classDeclaration(code);
				break;

			case OtScanner::OT_SCANNER_FUNCTION_TOKEN:
				functionDeclaration(code);
				break;

			case OtScanner::OT_SCANNER_DO_TOKEN:
				doStatement(code);
				code->push(nullptr);
				break;

			case OtScanner::OT_SCANNER_FOR_TOKEN:
				forStatement(code);
				code->push(nullptr);
				break;

			case OtScanner::OT_SCANNER_IF_TOKEN:
				ifStatement(code);
				code->push(nullptr);
				break;

			case OtScanner::OT_SCANNER_RETURN_TOKEN:
				returnStatement(code);
				break;

			case OtScanner::OT_SCANNER_SWITCH_TOKEN:
				switchStatement(code);
				code->push(nullptr);
				break;

			case OtScanner::OT_SCANNER_WHILE_TOKEN:
				whileStatement(code);
				code->push(nullptr);
				break;

			default:
				expression(code);
				break;
		}

		while (scanner.matchToken(OtScanner::OT_SCANNER_SEMICOLON_TOKEN))
			scanner.advance();
	}

private:
	// scanner for compilation
	OtScanner scanner;
};
