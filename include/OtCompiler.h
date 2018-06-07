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
//
//  OtContextReferenceClass
//

class OtContextReferenceClass : public OtObjectClass
{
public:
	OtContextReferenceClass() {}
	OtContextReferenceClass(const std::string& m) { member = m; }

	OtObject deref(OtObject context, size_t, OtObject*) { return context->get(member); }
	OtObject assign(OtObject context, size_t, OtObject* value) { return context->set(member, *value); }

	// get type definition
	static OtType getMeta()
	{
		static OtType type = nullptr;

		if (!type)
		{
			type = OtTypeClass::create<OtContextReferenceClass>("ContextReference", OtObjectClass::getMeta());
			type->set("__deref__", OtFunctionClass::create(&OtContextReferenceClass::deref));
			type->set("__assign__", OtFunctionClass::create(&OtContextReferenceClass::assign));
		}

		return type;
	}

	// create a new object
	static OtObject create(const std::string& n) { return std::make_shared<OtContextReferenceClass>(n)->setType(getMeta()); }

private:
	std::string member;
};


//
//  OtCompiler
//

class OtCompiler
{
public:
	// constructor
	OtCompiler() {}

	// compile text into microcode
	OtCode compile(const std::string& text)
	{
		// load scanner
		scanner.loadText(text);

		// setup code
		OtCode code = OtCodeClass::create();

		// process all statements
		if (scanner.matchToken(OtScanner::EOS_TOKEN))
			code->push(nullptr);

		else
		{
			statement(code);

			while (!scanner.matchToken(OtScanner::EOS_TOKEN))
			{
				code->pop();
				statement(code);
			}
		}

		// return compiled code
		return code;
	}

private:
	// compile function
	void function(OtCode code)
	{
		// create function
		OtCode function = OtCodeClass::create();
		function->saveArgs();

		// parse parameters
		size_t count = 0;
		scanner.expect(OtScanner::LPAREN_TOKEN);

		// handle ellipsis for variable argument count
		if (scanner.matchToken(OtScanner::ELLIPSIS_TOKEN))
		{
			scanner.advance();
			scanner.expect(OtScanner::RPAREN_TOKEN);
			count = SIZE_MAX;
		}

		else
		{
			// get parameter names
			while (!scanner.matchToken(OtScanner::RPAREN_TOKEN) && !scanner.matchToken(OtScanner::EOS_TOKEN))
			{
				scanner.expect(OtScanner::IDENTIFIER_TOKEN, false);
				function->saveArg(count++, scanner.getText());
				scanner.advance();

				if (scanner.matchToken(OtScanner::COMMA_TOKEN))
					scanner.advance();
			}

			scanner.expect(OtScanner::RPAREN_TOKEN);
		}

		// get function level code
		statement(function);
		code->push(OtCodeFunctionClass::create(function));
	}

	// compile primary expression
	bool primary(OtCode code)
	{
		bool reference;
		size_t count = 0;

		switch (scanner.getToken())
		{
			case OtScanner::LPAREN_TOKEN:
				// handle sub-expression
				scanner.advance();
				reference = expression(code);
				scanner.expect(OtScanner::RPAREN_TOKEN);
				break;

			case OtScanner::INTEGER_TOKEN:
				// handle integer constants
				code->push(OtObjectCreate(scanner.getInteger()));
				scanner.advance();
				reference = false;
				break;

			case OtScanner::REAL_TOKEN:
				// handle real constants
				code->push(OtObjectCreate(scanner.getReal()));
				scanner.advance();
				reference = false;
				break;

			case OtScanner::STRING_TOKEN:
				// handle string constants
				code->push(OtObjectCreate(scanner.getString()));
				scanner.advance();
				reference = false;
				break;

			case OtScanner::FUNCTION_TOKEN:
				// handle function definition
				scanner.advance();
				function(code);
				reference = false;
				break;

			case OtScanner::IDENTIFIER_TOKEN:
				// handle named reference
				code->push(OtContextReferenceClass::create(scanner.getText()));
				scanner.advance();
				reference = true;
				break;

			case OtScanner::LBRACKET_TOKEN:
				// handle array constant
				scanner.advance();
				code->push(OtArrayClass::create());
				code->method("__init__", expressions(code));
				scanner.expect(OtScanner::RBRACKET_TOKEN);
				reference = false;
				break;

			case OtScanner::LBRACE_TOKEN:
				// handle dictionary constant
				scanner.advance();
				code->push(OtDictClass::create());

				while (scanner.getToken() != OtScanner::RBRACE_TOKEN && scanner.getToken() != OtScanner::EOS_TOKEN)
				{
					scanner.expect(OtScanner::IDENTIFIER_TOKEN, false);
					code->push(OtObjectCreate(scanner.getText()));
					scanner.advance();
					scanner.expect(OtScanner::COLON_TOKEN);

					if (expression(code))
						code->method("__deref__", 0);

					count += 2;

					if (scanner.getToken() == OtScanner::COMMA_TOKEN)
						scanner.advance();
				}

				scanner.expect(OtScanner::RBRACE_TOKEN);
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
		while (token == OtScanner::LBRACKET_TOKEN ||
			   token == OtScanner::LPAREN_TOKEN ||
			   token == OtScanner::PERIOD_TOKEN ||
			   token == OtScanner::INCREMENT_TOKEN ||
			   token == OtScanner::DECREMENT_TOKEN)
		{
			scanner.advance();
			std::string member;
			size_t count;

			// generate code
			switch (token)
			{
				case OtScanner::LBRACKET_TOKEN:
					// index object
					if (reference)
						code->method("__deref__", 0);

					if (expression(code))
						code->method("__deref__", 0);

					scanner.expect(OtScanner::RBRACKET_TOKEN);
					code->method("__index__", 1);
					reference = true;
					break;

				case OtScanner::LPAREN_TOKEN:
					// call object
					if (reference)
						code->method("__deref__", 0);

					count = scanner.matchToken(OtScanner::RPAREN_TOKEN) ? 0 : expressions(code);
					scanner.expect(OtScanner::RPAREN_TOKEN);
					code->method("__call__", count);
					reference = false;
					break;

				case OtScanner::PERIOD_TOKEN:
					// member access
					if (reference)
						code->method("__deref__", 0);

					scanner.expect(OtScanner::IDENTIFIER_TOKEN, false);
					code->push(OtObjectCreate(scanner.getText()));
					scanner.advance();
					code->method("__member__", 1);
					reference = true;

					break;

				case OtScanner::INCREMENT_TOKEN:
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

				case OtScanner::DECREMENT_TOKEN:
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
		if (token == OtScanner::SUBTRACT_TOKEN ||
			token == OtScanner::NOT_TOKEN ||
			token == OtScanner::BITWISE_NOT_TOKEN ||
			token == OtScanner::INCREMENT_TOKEN ||
			token == OtScanner::DECREMENT_TOKEN)
		{
			scanner.advance();
			bool reference = postfix(code);

			// generate code
			switch (token)
			{
				case OtScanner::SUBTRACT_TOKEN:
					if (reference)
						code->method("__deref__", 0);

					code->method("__neg__", 0);
					break;

				case OtScanner::NOT_TOKEN:
					if (reference)
						code->method("__deref__", 0);

					code->method("__not__", 0);
					break;

				case OtScanner::BITWISE_NOT_TOKEN:
					if (reference)
						code->method("__deref__", 0);

					code->method("__bnot__", 0);
					break;

				case OtScanner::INCREMENT_TOKEN:
					if (!reference)
						scanner.error("Lvalue required for '++'");

					code->dup();
					code->method("__deref__", 0);
					code->method("__inc__", 0);
					code->method("__assign__", 0);
					break;

				case OtScanner::DECREMENT_TOKEN:
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

		while (token == OtScanner::MULTIPLY_TOKEN ||
			   token == OtScanner::DIVIDE_TOKEN ||
			   token == OtScanner::POWER_TOKEN ||
			   token == OtScanner::MODULO_TOKEN)
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
				case OtScanner::MULTIPLY_TOKEN:
					code->method("__mul__", 1);
					break;

				case OtScanner::DIVIDE_TOKEN:
					code->method("__div__", 1);
					break;

				case OtScanner::POWER_TOKEN:
					code->method("__power__", 1);
					break;

				case OtScanner::MODULO_TOKEN:
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

		while (token == OtScanner::ADD_TOKEN || token == OtScanner::SUBTRACT_TOKEN)
		{
			// deref if required
			scanner.advance();

			if (reference)
				code->method("__deref__", 0);

			// parse right side
			if (multiplications(code))
				code->method("__deref__", 0);

			// generate code
			if (token == OtScanner::ADD_TOKEN)
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

		while (token == OtScanner::SHIFT_LEFT_TOKEN || token == OtScanner::SHIFT_RIGHT_TOKEN)
		{
			// deref if required
			scanner.advance();

			if (reference)
				code->method("__deref__", 0);

			// parse right side
			if (additions(code))
				code->method("__deref__", 0);

			// generate code
			if (token == OtScanner::SHIFT_LEFT_TOKEN)
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

		while (token == OtScanner::LESS_TOKEN ||
			   token == OtScanner::LESS_EQUAL_TOKEN ||
			   token == OtScanner::GREATER_TOKEN ||
			   token == OtScanner::GREATER_EQUAL_TOKEN)
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
				case OtScanner::LESS_TOKEN:
					code->method("__lt__", 1);
					break;

				case OtScanner::LESS_EQUAL_TOKEN:
					code->method("__le__", 1);
					break;

				case OtScanner::GREATER_TOKEN:
					code->method("__gt__", 1);
					break;

				case OtScanner::GREATER_EQUAL_TOKEN:
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

		while (token == OtScanner::EQUAL_TOKEN || token == OtScanner::NOT_EQUAL_TOKEN)
		{
			// deref if required
			scanner.advance();

			if (reference)
				code->method("__deref__", 0);

			// parse right side
			if (relation(code))
				code->method("__deref__", 0);

			// generate code
			if (token == OtScanner::EQUAL_TOKEN)
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
		if (scanner.matchToken(OtScanner::BITWISE_AND_TOKEN))
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
		if (scanner.matchToken(OtScanner::BITWISE_XOR_TOKEN))
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
		if (scanner.matchToken(OtScanner::BITWISE_OR_TOKEN))
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
		while (scanner.matchToken(OtScanner::AND_TOKEN))
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
		while (scanner.matchToken(OtScanner::OR_TOKEN))
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

		if (scanner.matchToken(OtScanner::QUESTION_TOKEN))
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

			scanner.expect(OtScanner::COLON_TOKEN);

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
		while (token == OtScanner::ASSIGNMENT_TOKEN ||
			   token == OtScanner::MULTIPLY_ASSIGNMENT_TOKEN ||
			   token == OtScanner::DIVIDE_ASSIGNMENT_TOKEN ||
			   token == OtScanner::MODULO_ASSIGNMENT_TOKEN ||
			   token == OtScanner::ADD_ASSIGNMENT_TOKEN ||
			   token == OtScanner::SUBTRACT_ASSIGNMENT_TOKEN ||
			   token == OtScanner::BITWISE_AND_ASSIGNMENT_TOKEN ||
			   token == OtScanner::BITWISE_OR_ASSIGNMENT_TOKEN ||
			   token == OtScanner::BITWISE_XOR_ASSIGNMENT_TOKEN)
		{
			// sanity check
			if (!reference)
				scanner.error("Lvalue required for assignments");

			// duplicate left side if required
			if (token != OtScanner::ASSIGNMENT_TOKEN)
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
				case OtScanner::MULTIPLY_ASSIGNMENT_TOKEN:
					code->method("__mul__", 1);
					break;

				case OtScanner::DIVIDE_ASSIGNMENT_TOKEN:
					code->method("__div__", 1);
					break;

				case OtScanner::MODULO_ASSIGNMENT_TOKEN:
					code->method("__mod__", 1);
					break;

				case OtScanner::ADD_ASSIGNMENT_TOKEN:
					code->method("__add__", 1);
					break;

				case OtScanner::SUBTRACT_ASSIGNMENT_TOKEN:
					code->method("__sub__", 1);
					break;

				case OtScanner::BITWISE_AND_ASSIGNMENT_TOKEN:
					code->method("__bend__", 1);
					break;

				case OtScanner::BITWISE_OR_ASSIGNMENT_TOKEN:
					code->method("__bor__", 1);
					break;

				case OtScanner::BITWISE_XOR_ASSIGNMENT_TOKEN:
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

		while (scanner.matchToken(OtScanner::COMMA_TOKEN))
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
		scanner.expect(OtScanner::LBRACE_TOKEN);

		if (scanner.matchToken(OtScanner::RBRACE_TOKEN))
			code->push(nullptr);

		else
		{
			statement(code);

			while (!scanner.matchToken(OtScanner::RBRACE_TOKEN) && !scanner.matchToken(OtScanner::EOS_TOKEN))
			{
				code->pop();
				statement(code);
			}
		}

		scanner.expect(OtScanner::RBRACE_TOKEN);
	}

	// compile a class definition
	void classDeclaration(OtCode code)
	{
		// handle class name
		scanner.expect(OtScanner::CLASS_TOKEN);
		scanner.expect(OtScanner::IDENTIFIER_TOKEN, false);
		std::string name = scanner.getText();
		scanner.advance();

		// handle parent name
		scanner.expect(OtScanner::COLON_TOKEN);
		scanner.expect(OtScanner::IDENTIFIER_TOKEN, false);
		std::string parent = scanner.getText();
		scanner.advance();

		// create new class
		code->get(parent);
		code->push(OtObjectCreate(name));
		code->method("subType", 1);

		// assign class
		code->dup();
		code->set(name);

		// process class content
		code->dup();
		code->pushContext();
		statement(code);
		code->pop();
		code->popContext();
	}

	// compile function definition
	void functionDeclaration(OtCode code)
	{
		// get function name
		scanner.expect(OtScanner::FUNCTION_TOKEN);
		scanner.expect(OtScanner::IDENTIFIER_TOKEN, false);
		std::string name = scanner.getText();
		scanner.advance();

		// parse function and add to context
		function(code);
		code->dup();
		code->set(name);
	}

	// compile a do statement
	void doStatement(OtCode code)
	{
		scanner.expect(OtScanner::DO_TOKEN);
		size_t offset = code->size();

		statement(code);
		code->pop();

		scanner.expect(OtScanner::WHILE_TOKEN);

		if (expression(code))
			code->method("__deref__", 0);

		code->jumpTrue(offset);
	}

	// compile a for statement
	void forStatement(OtCode code)
	{
		scanner.expect(OtScanner::FOR_TOKEN);
		scanner.expect(OtScanner::LPAREN_TOKEN);

		if (!scanner.matchToken(OtScanner::SEMICOLON_TOKEN))
			code->pop(expressions(code));

		scanner.expect(OtScanner::SEMICOLON_TOKEN);

		size_t offset1 = code->size();

		if (expression(code))
			code->method("__deref__", 0);

		size_t offset2 = code->size();
		code->jumpFalse(0);

		scanner.expect(OtScanner::SEMICOLON_TOKEN);

		OtCode tmp = OtCodeClass::create();

		if (!scanner.matchToken(OtScanner::RPAREN_TOKEN))
			tmp->pop(expressions(tmp));

		scanner.expect(OtScanner::RPAREN_TOKEN);
		statement(code);
		code->pop();

		code->insert(code->end(), tmp->begin(), tmp->end());

		code->jump(offset1);
		code->patch(offset2);
	}

	// compile an if statement
	void ifStatement(OtCode code)
	{
		scanner.expect(OtScanner::IF_TOKEN);

		if (expression(code))
			code->method("__deref__", 0);

		size_t offset1 = code->size();
		code->jumpFalse(0);

		statement(code);
		code->pop();

		if (scanner.matchToken(OtScanner::ELSE_TOKEN))
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
		scanner.expect(OtScanner::RETURN_TOKEN);

		if (expression(code))
			code->method("__deref__", 0);

		code->exit();
	}

	// compile a switch statement
	void switchStatement(OtCode code)
	{
		std::vector<size_t> patches;

		scanner.expect(OtScanner::SWITCH_TOKEN);

		if (expression(code))
			code->method("__deref__", 0);

			while (scanner.matchToken(OtScanner::CASE_TOKEN))
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

		if (scanner.matchToken(OtScanner::DEFAULT_TOKEN))
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
		scanner.expect(OtScanner::WHILE_TOKEN);
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
			case OtScanner::LBRACE_TOKEN:
				block(code);
				break;

			case OtScanner::CLASS_TOKEN:
				classDeclaration(code);
				break;

			case OtScanner::FUNCTION_TOKEN:
				functionDeclaration(code);
				break;

			case OtScanner::DO_TOKEN:
				doStatement(code);
				code->push(nullptr);
				break;

			case OtScanner::FOR_TOKEN:
				forStatement(code);
				code->push(nullptr);
				break;

			case OtScanner::IF_TOKEN:
				ifStatement(code);
				code->push(nullptr);
				break;

			case OtScanner::RETURN_TOKEN:
				returnStatement(code);
				break;

			case OtScanner::SWITCH_TOKEN:
				switchStatement(code);
				code->push(nullptr);
				break;

			case OtScanner::WHILE_TOKEN:
				whileStatement(code);
				code->push(nullptr);
				break;

			default:
				expression(code);
				break;
		}

		while (scanner.matchToken(OtScanner::SEMICOLON_TOKEN))
			scanner.advance();
	}

private:
	// scanner for compilation
	OtScanner scanner;
};
