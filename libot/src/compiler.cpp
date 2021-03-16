//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <vector>

#include "ot/compiler.h"
#include "ot/exception.h"
#include "ot/function.h"

#include "ot/array.h"
#include "ot/dict.h"

#include "ot/reference.h"


//
//	OtCompiler::compile
//

OtCode OtCompiler::compile(const std::string& text) {
	// load scanner
	scanner.loadText(text);

	// setup code
	OtCode code = OtCodeClass::create(text);

	// process all statements
	if (scanner.matchToken(OtScanner::EOS_TOKEN)) {
		code->push(nullptr);

	} else {
		statement(code);

		while (!scanner.matchToken(OtScanner::EOS_TOKEN)) {
			code->pop();
			statement(code);
		}
	}

	// return compiled code
	return code;
}


//
//	OtCodeFunction
//

class OtCodeFunctionClass;
typedef std::shared_ptr<OtCodeFunctionClass> OtCodeFunction;


class OtCodeFunctionClass : public OtInternalClass {
public:
	// constructor
	OtCodeFunctionClass() = default;
	OtCodeFunctionClass(size_t p, const std::vector<std::string>& n, std::weak_ptr<OtCodeClass> c, size_t s, size_t e)
		: parameterCount(p), names(n), code(c), start(s), end(e) {}

	// call code
	OtObject operator () (OtContext context, size_t count, OtObject* parameters) {
		// sanity check
		if (parameterCount != SIZE_MAX && parameterCount != count) {
			OT_EXCEPT("Function expects %d parameters, %d given", parameterCount, count);
		}

		// create local context
		OtContext local = OtContextClass::create();
		local->setParent(context);

		// set calling parameters
		local->set("args", OtArrayClass::create(count, parameters));

		for (auto c= 0; c < count; c++) {
			local->set(names[c], parameters[c]);
		}

		return code.lock()->operator ()(local, start, end);
	}

	// get type definition
	static OtType getMeta() {
		static OtType type = nullptr;

		if (!type) {
			type = OtTypeClass::create<OtCodeFunctionClass>("CodeFunction", OtInternalClass::getMeta());
			type->set("__call__", OtFunctionClass::create(&OtCodeFunctionClass::operator ()));
		}

		return type;
	}

	// create a new object
	static OtCodeFunction create(size_t count, const std::vector<std::string>& names, std::weak_ptr<OtCodeClass> code, size_t start, size_t end) {
		OtCodeFunction func = std::make_shared<OtCodeFunctionClass>(count, names, code, start, end);
		func->setType(getMeta());
		return func;
	}

private:
	size_t parameterCount;
	std::vector<std::string> names;
	std::weak_ptr<OtCodeClass> code;
	size_t start;
	size_t end;
};


//
//	OtCompiler::function
//

void OtCompiler::function(OtCode code) {
	// jump around function code
	size_t offset = code->size();
	code->jump(0);

	// parse parameters
	size_t count = 0;
	std::vector<std::string> names;

	scanner.expect(OtScanner::LPAREN_TOKEN);

	// handle ellipsis for variable argument count
	if (scanner.matchToken(OtScanner::ELLIPSIS_TOKEN)) {
		scanner.advance();
		scanner.expect(OtScanner::RPAREN_TOKEN);
		count = SIZE_MAX;

	} else {
		// get parameter names
		while (!scanner.matchToken(OtScanner::RPAREN_TOKEN) && !scanner.matchToken(OtScanner::EOS_TOKEN)) {
			scanner.expect(OtScanner::IDENTIFIER_TOKEN, false);
			names.push_back(scanner.getText());
			scanner.advance();

			if (!scanner.matchToken(OtScanner::RPAREN_TOKEN)) {
				scanner.expect(OtScanner::COMMA_TOKEN);
			}
		}

		scanner.expect(OtScanner::RPAREN_TOKEN);
		count = names.size();
	}

	// get function level code
	size_t start = code->size();
	block(code);
	code->patch(offset);

	// put new function on stack
	code->push(OtCodeFunctionClass::create(count, names, code, start, code->size()));
}


//
// OtContextReferenceClass
//

class OtContextReferenceClass;
typedef std::shared_ptr<OtContextReferenceClass> OtContextReference;

class OtContextReferenceClass : public OtReferenceClass {
public:
	OtContextReferenceClass() = default;
	OtContextReferenceClass(const std::string& m) : member(m) {}

	OtObject deref(OtContext context, size_t, OtObject*) { return context->get(member); }
	OtObject assign(OtContext context, size_t, OtObject* value) { return context->set(member, *value); }

	// get type definition
	static OtType getMeta() {
		static OtType type = nullptr;

		if (!type) {
			type = OtTypeClass::create<OtContextReferenceClass>("ContextReference", OtReferenceClass::getMeta());
			type->set("__deref__", OtFunctionClass::create(&OtContextReferenceClass::deref));
			type->set("__assign__", OtFunctionClass::create(&OtContextReferenceClass::assign));
		}

		return type;
	}

	// create a new object
	static OtContextReference create(const std::string& n) {
		OtContextReference reference = std::make_shared<OtContextReferenceClass>(n);
		reference->setType(getMeta());
		return reference;
	}

private:
	std::string member;
};


//
//	OtCompiler::primary
//

bool OtCompiler::primary(OtCode code) {
	bool reference;
	size_t count = 0;

	switch (scanner.getToken()) {
		case OtScanner::LPAREN_TOKEN:
			// handle sub-expression
			scanner.advance();
			reference = expression(code);
			scanner.expect(OtScanner::RPAREN_TOKEN);
			break;

		case OtScanner::INTEGER_TOKEN:
			// handle integer constants
			code->push(OtIntegerClass::create(scanner.getInteger()));
			scanner.advance();
			reference = false;
			break;

		case OtScanner::REAL_TOKEN:
			// handle real constants
			code->push(OtRealClass::create(scanner.getReal()));
			scanner.advance();
			reference = false;
			break;

		case OtScanner::STRING_TOKEN:
			// handle string constants
			code->push(OtStringClass::create(scanner.getString()));
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

			if (scanner.getToken() != OtScanner::RBRACKET_TOKEN) {
				code->method("__init__", expressions(code));
			}

			scanner.expect(OtScanner::RBRACKET_TOKEN);
			reference = false;
			break;

		case OtScanner::LBRACE_TOKEN:
			// handle dictionary constant
			scanner.advance();
			code->push(OtDictClass::create());

			while (scanner.getToken() != OtScanner::RBRACE_TOKEN && scanner.getToken() != OtScanner::EOS_TOKEN) {
				scanner.expect(OtScanner::STRING_TOKEN, false);
				code->push(OtStringClass::create(scanner.getString()));
				scanner.advance();
				scanner.expect(OtScanner::COLON_TOKEN);

				if (expression(code)) {
					code->method("__deref__", 0);
				}

				count += 2;

				if (scanner.getToken() == OtScanner::COMMA_TOKEN) {
					scanner.advance();
				}
			}

			scanner.expect(OtScanner::RBRACE_TOKEN);

			if (count) {
				code->method("__init__", count);
			}

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

bool OtCompiler::postfix(OtCode code) {
	// parse left side
	bool reference = primary(code);
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
					code->method("__deref__", 0);
				}

				if (expression(code)) {
					code->method("__deref__", 0);
				}

				scanner.expect(OtScanner::RBRACKET_TOKEN);
				code->method("__index__", 1);
				reference = true;
				break;

			case OtScanner::LPAREN_TOKEN:
				// call object
				if (reference) {
					code->method("__deref__", 0);
				}

				count = scanner.matchToken(OtScanner::RPAREN_TOKEN) ? 0 : expressions(code);
				scanner.expect(OtScanner::RPAREN_TOKEN);
				code->method("__call__", count);
				reference = false;
				break;

			case OtScanner::PERIOD_TOKEN:
				// member access
				if (reference) {
					code->method("__deref__", 0);
				}

				scanner.expect(OtScanner::IDENTIFIER_TOKEN, false);
				code->push(OtStringClass::create(scanner.getText()));
				scanner.advance();
				code->method("__member__", 1);
				reference = true;

				break;

			case OtScanner::INCREMENT_TOKEN:
				if (!reference) {
					scanner.error("Lvalue required for '++'");
				}

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
				if (!reference) {
					scanner.error("Lvalue required for '--'");
				}

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


//
//	OtCompiler::prefix
//

bool OtCompiler::prefix(OtCode code) {
	OtToken token = scanner.getToken();

	// process prefix
	if (token == OtScanner::SUBTRACT_TOKEN ||
		token == OtScanner::NEGATE_TOKEN ||
		token == OtScanner::BITWISE_NOT_TOKEN ||
		token == OtScanner::INCREMENT_TOKEN ||
		token == OtScanner::DECREMENT_TOKEN) {
		scanner.advance();
		bool reference = postfix(code);

		// generate code
		switch (token) {
			case OtScanner::SUBTRACT_TOKEN:
				if (reference) {
					code->method("__deref__", 0);
				}

				code->method("__neg__", 0);
				break;

			case OtScanner::NEGATE_TOKEN:
				if (reference) {
					code->method("__deref__", 0);
				}

				code->method("__not__", 0);
				break;

			case OtScanner::BITWISE_NOT_TOKEN:
				if (reference) {
					code->method("__deref__", 0);
				}

				code->method("__bnot__", 0);
				break;

			case OtScanner::INCREMENT_TOKEN:
				if (!reference) {
					scanner.error("Lvalue required for '++'");
				}

				code->dup();
				code->method("__deref__", 0);
				code->method("__inc__", 0);
				code->method("__assign__", 0);
				break;

			case OtScanner::DECREMENT_TOKEN:
				if (!reference) {
					scanner.error("Lvalue required for '--'");
				}

				code->dup();
				code->method("__deref__", 0);
				code->method("__dec__", 0);
				code->method("__assign__", 0);
				break;
		}

		return false;

	} else {
		return postfix(code);
	}
}


//
//	OtCompiler::multiplications
//

bool OtCompiler::multiplications(OtCode code) {
	// parse left side
	bool reference = prefix(code);
	OtToken token = scanner.getToken();

	while (token == OtScanner::MULTIPLY_TOKEN ||
		   token == OtScanner::DIVIDE_TOKEN ||
		   token == OtScanner::POWER_TOKEN ||
		   token == OtScanner::MODULO_TOKEN) {
		// deref if required
		scanner.advance();

		if (reference) {
			code->method("__deref__", 0);
		}

		// parse right side
		if (prefix(code)) {
			code->method("__deref__", 0);
		}

		// generate code
		switch (token) {
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


//
//	OtCompiler::additions
//

bool OtCompiler::additions(OtCode code) {
	// parse left side
	bool reference = multiplications(code);
	OtToken token = scanner.getToken();

	while (token == OtScanner::ADD_TOKEN || token == OtScanner::SUBTRACT_TOKEN) {
		// deref if required
		scanner.advance();

		if (reference) {
			code->method("__deref__", 0);
		}

		// parse right side
		if (multiplications(code)) {
			code->method("__deref__", 0);
		}

		// generate code
		if (token == OtScanner::ADD_TOKEN) {
			code->method("__add__", 1);
		} else {
			code->method("__sub__", 1);
		}

		token = scanner.getToken();
		reference = false;
	}

	return reference;
}


//
//	OtCompiler::shift
//

bool OtCompiler::shift(OtCode code) {
	// parse left side
	bool reference = additions(code);
	OtToken token = scanner.getToken();

	while (token == OtScanner::SHIFT_LEFT_TOKEN || token == OtScanner::SHIFT_RIGHT_TOKEN) {
		// deref if required
		scanner.advance();

		if (reference) {
			code->method("__deref__", 0);
		}

		// parse right side
		if (additions(code)) {
			code->method("__deref__", 0);
		}

		// generate code
		if (token == OtScanner::SHIFT_LEFT_TOKEN) {
			code->method("__lshift__", 1);
		} else {
			code->method("__rshift__", 1);
		}

		token = scanner.getToken();
		reference = false;
	}

	return reference;
}


//
//	OtCompiler::relation
//

bool OtCompiler::relation(OtCode code) {
	// parse left side
	bool reference = shift(code);
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
			code->method("__deref__", 0);
		}

		// parse right side
		if (shift(code)) {
			code->method("__deref__", 0);
		}

		// generate code
		switch (token) {
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

			case OtScanner::IN_TOKEN:
				code->swap();
				code->method("__contains__", 1);
				break;

			case OtScanner::NOT_TOKEN:
				code->swap();
				code->method("__contains__", 1);
				code->method("__not__", 0);
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

bool OtCompiler::equal(OtCode code) {
	// parse left side
	bool reference = relation(code);
	OtToken token = scanner.getToken();

	while (token == OtScanner::EQUAL_TOKEN || token == OtScanner::NOT_EQUAL_TOKEN) {
		scanner.advance();

		// deref if required
		if (reference) {
			code->method("__deref__", 0);
		}

		// parse right side
		if (relation(code)) {
			code->method("__deref__", 0);
		}

		// generate code
		if (token == OtScanner::EQUAL_TOKEN) {
			code->method("__eq__", 1);
		} else {
			code->method("__ne__", 1);
		}

		token = scanner.getToken();
		reference = false;
	}

	return reference;
}


//
//	OtCompiler::bitwiseAnd
//

bool OtCompiler::bitwiseAnd(OtCode code) {
	// parse left side
	bool reference = equal(code);

	// handle all "&" operators
	if (scanner.matchToken(OtScanner::BITWISE_AND_TOKEN)) {
		// deref if required
		scanner.advance();

		if (reference) {
			code->method("__deref__", 0);
		}

		// parse right side
		if (equal(code)) {
			code->method("__deref__", 0);
		}

		// generate code
		code->method("__band__", 1);
		reference = false;
	}

	return reference;
}


//
//	OtCompiler::bitwiseXor
//

bool OtCompiler::bitwiseXor(OtCode code) {
	// parse left side
	bool reference = bitwiseAnd(code);

	// handle all "&" operators
	if (scanner.matchToken(OtScanner::BITWISE_XOR_TOKEN)) {
		// deref if required
		scanner.advance();

		if (reference) {
			code->method("__deref__", 0);
		}

		// parse right side
		if (bitwiseAnd(code)) {
			code->method("__deref__", 0);
		}

		// generate code
		code->method("__bxor__", 1);
		reference = false;
	}

	return reference;
}


//
//	OtCompiler::bitwiseOr
//

bool OtCompiler::bitwiseOr(OtCode code) {
	// parse left side
	bool reference = bitwiseXor(code);

	// handle all "|" operators
	if (scanner.matchToken(OtScanner::BITWISE_OR_TOKEN)) {
		// deref if required
		scanner.advance();

		if (reference) {
			code->method("__deref__", 0);
		}

		// parse right side
		if (bitwiseXor(code)) {
			code->method("__deref__", 0);
		}

		// generate code
		code->method("__bor__", 1);
		reference = false;
	}

	return reference;
}


//
//	OtCompiler::andExpression
//

bool OtCompiler::andExpression(OtCode code) {
	// parse left side
	bool reference = bitwiseOr(code);

	// handle all "&&" operators
	while (scanner.matchToken(OtScanner::AND_TOKEN)) {
		// skip token
		scanner.advance();

		// deref if required
		if (reference) {
			code->method("__deref__", 0);
		}

		// parse right side
		if (bitwiseOr(code)) {
			code->method("__deref__", 0);
		}

		// generate code
		code->method("__and__", 1);
		reference = false;
	}

	return reference;
}


//
//	OtCompiler::orExpression
//

bool OtCompiler::orExpression(OtCode code) {
	// parse left side
	bool reference = andExpression(code);

	// handle all "||" operators
	while (scanner.matchToken(OtScanner::OR_TOKEN)) {
		// skip token
		scanner.advance();

		// deref if required
		if (reference) {
			code->method("__deref__", 0);
		}

		// parse right side
		if (andExpression(code)) {
			code->method("__deref__", 0);
		}

		// generate code
		code->method("__or__", 1);
		reference = false;
	}

	return reference;
}


//
//	OtCompiler::conditional
//

bool OtCompiler::conditional(OtCode code) {
	// parse left side
	bool reference = orExpression(code);

	if (scanner.matchToken(OtScanner::QUESTION_TOKEN)) {
		// deref if required
		scanner.advance();

		if (reference) {
			code->method("__deref__", 0);
		}

		// evaluate expression
		size_t offset1 = code->size();
		code->jumpFalse(0);

		// process "true" expression
		if (expression(code)) {
			code->method("__deref__", 0);
		}

		scanner.expect(OtScanner::COLON_TOKEN);

		// jump around "false" expression and patch first jump
		size_t offset2 = code->size();
		code->jump(0);
		code->patch(offset1);

		// process "false" expression
		if (expression(code)) {
			code->method("__deref__", 0);
		}

		// patch jump around false
		code->patch(offset2);
		reference = false;
	}

	return reference;
}


//
//	OtCompiler::expression
//

bool OtCompiler::expression(OtCode code) {
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
		   token == OtScanner::BITWISE_XOR_ASSIGNMENT_TOKEN) {
		// sanity check
		if (!reference) {
			scanner.error("Lvalue required for assignments");
		}

		// duplicate left side if required
		if (token != OtScanner::ASSIGNMENT_TOKEN) {
			code->dup();
			code->method("__deref__", 0);
		}

		// parse right side
		scanner.advance();
		reference = conditional(code);

		// dereference if required
		if (reference) {
			code->method("__deref__", 0);
		}

		// handle different assignments
		switch (token) {
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
				code->method("__band__", 1);
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


//
//	OtCompiler::expressions
//

size_t OtCompiler::expressions(OtCode code) {
	// process comma seperated list of expressions
	size_t count = 1;

	if (expression(code)) {
		code->method("__deref__", 0);
	}

	while (scanner.matchToken(OtScanner::COMMA_TOKEN)) {
		scanner.advance();

		if (expression(code)) {
			code->method("__deref__", 0);
		}

		count++;
	}

	return count;
}


//
//	OtCompiler::block
//

void OtCompiler::block(OtCode code) {
	scanner.expect(OtScanner::LBRACE_TOKEN);

	if (scanner.matchToken(OtScanner::RBRACE_TOKEN)) {
		code->push(nullptr);

	} else {
		statement(code);

		while (!scanner.matchToken(OtScanner::RBRACE_TOKEN) && !scanner.matchToken(OtScanner::EOS_TOKEN)) {
			code->pop();
			statement(code);
		}
	}

	scanner.expect(OtScanner::RBRACE_TOKEN);
}


//
//	OtCompiler::classDeclaration
//

void OtCompiler::classDeclaration(OtCode code) {
	// handle class name
	scanner.expect(OtScanner::CLASS_TOKEN);
	scanner.expect(OtScanner::IDENTIFIER_TOKEN, false);
	std::string name = scanner.getText();
	scanner.advance();

	// handle parent
	scanner.expect(OtScanner::COLON_TOKEN);

	if (expression(code)) {
		code->method("__deref__", 0);
	}

	// create new class
	code->push(OtStringClass::create(name));
	code->method("subClass", 1);

	// assign class
	code->push(OtContextReferenceClass::create(name));
	code->swap();
	code->method("__assign__", 1);

	// process class content
	code->dup();
	code->pushContext();
	block(code);
	code->pop();
	code->popContext();
}


//
//	OtCompiler::functionDeclaration
//

void OtCompiler::functionDeclaration(OtCode code) {
	// get function name
	scanner.expect(OtScanner::FUNCTION_TOKEN);
	scanner.expect(OtScanner::IDENTIFIER_TOKEN, false);
	std::string name = scanner.getText();
	scanner.advance();

	// parse function and add to context
	function(code);
	code->push(OtContextReferenceClass::create(name));
	code->swap();
	code->method("__assign__", 1);
}


//
//	OtCompiler::doStatement
//

void OtCompiler::doStatement(OtCode code) {
	scanner.expect(OtScanner::DO_TOKEN);
	size_t offset = code->size();

	block(code);
	code->pop();

	scanner.expect(OtScanner::WHILE_TOKEN);

	if (expression(code)) {
		code->method("__deref__", 0);
	}

	code->jumpTrue(offset);
}


//
//	OtCompiler::forStatement
//

void OtCompiler::forStatement(OtCode code) {
	scanner.expect(OtScanner::FOR_TOKEN);

	scanner.expect(OtScanner::IDENTIFIER_TOKEN, false);
	std::string name = scanner.getText();
	scanner.advance();

	scanner.expect(OtScanner::IN_TOKEN);

	if (expression(code)) {
		code->method("__deref__", 0);
	}

	code->method("__iter__", 0);
	size_t offset1 = code->size();

	code->dup();
	code->method("__end__", 0);
	size_t offset2 = code->size();
	code->jumpTrue(0);

	code->dup();
	code->method("__next__", 0);
	code->push(OtContextReferenceClass::create(name));
	code->swap();
	code->method("__assign__", 1);
	code->pop();

	block(code);
	code->pop();

	code->jump(offset1);
	code->patch(offset2);
	code->pop();
}

//
//	OtCompiler::ifStatement
//

void OtCompiler::ifStatement(OtCode code) {
	scanner.expect(OtScanner::IF_TOKEN);

	if (expression(code)) {
		code->method("__deref__", 0);
	}

	size_t offset1 = code->size();
	code->jumpFalse(0);

	block(code);
	code->pop();

	if (scanner.matchToken(OtScanner::ELSE_TOKEN)) {
		scanner.advance();

		size_t offset2 = code->size();
		code->jump(0);

		code->patch(offset1);
		block(code);
		code->pop();

		code->patch(offset2);

	} else {
		code->patch(offset1);
	}
}


//
//	OtCompiler::returnStatement
//

void OtCompiler::returnStatement(OtCode code) {
	scanner.expect(OtScanner::RETURN_TOKEN);

	if (expression(code)) {
		code->method("__deref__", 0);
	}

	code->exit();
}


//
//	OtCompiler::switchStatement
//

void OtCompiler::switchStatement(OtCode code) {
	std::vector<size_t> patches;

	scanner.expect(OtScanner::SWITCH_TOKEN);

	if (expression(code)) {
		code->method("__deref__", 0);
	}

	scanner.expect(OtScanner::LBRACE_TOKEN);

	while (scanner.matchToken(OtScanner::CASE_TOKEN)) {
		scanner.advance();
		code->dup();

		if (expression(code)) {
			code->method("__deref__", 0);
		}

		code->method("__eq__", 1);

		size_t offset1 = code->size();
		code->jumpFalse(0);

		statement(code);
		code->pop();

		patches.push_back(code->size());
		code->jump(0);

		code->patch(offset1);
	}

	if (scanner.matchToken(OtScanner::DEFAULT_TOKEN)) {
		scanner.advance();
		statement(code);
		code->pop();
	}

	scanner.expect(OtScanner::RBRACE_TOKEN);

	for (auto const& patch : patches) {
		code->patch(patch);
	}

	code->pop();
}


//
// OtHttpNotFound
//

class OtThrowClass;
typedef std::shared_ptr<OtThrowClass> OtThrow;

class OtThrowClass : public OtInternalClass {
public:
	OtThrowClass() = default;

	// throw the exception
	void call(const std::string& error) {
		throw OtException(error);
	}

	// get type definition
	static OtType getMeta() {
		static OtType type = nullptr;

		if (!type) {
			type = OtTypeClass::create<OtThrowClass>("Throw", OtInternalClass::getMeta());
			type->set("__call__", OtFunctionClass::create(&OtThrowClass::call));
		}

		return type;
	}

	// create a new object
	static OtThrow create() {
		OtThrow thrw = std::make_shared<OtThrowClass>();
		thrw->setType(getMeta());
		return thrw;
	}
};


//
//	OtCompiler::throwStatement
//

void OtCompiler::throwStatement(OtCode code) {
	scanner.expect(OtScanner::THROW_TOKEN);
	code->push(OtThrowClass::create());

	if (expression(code)) {
		code->method("__deref__", 0);
	}

	code->method("__call__", 1);
}


//
//	OtCompiler::tryStatement
//

void OtCompiler::tryStatement(OtCode code) {
	scanner.expect(OtScanner::TRY_TOKEN);

	size_t offset1 = code->size();
	code->pushTry();

	block(code);
	code->pop();

	code->popTry();

	size_t offset2 = code->size();
	code->jump(0);
	code->patch(offset1);

	scanner.expect(OtScanner::CATCH_TOKEN);
	scanner.expect(OtScanner::IDENTIFIER_TOKEN, false);
	code->push(OtContextReferenceClass::create(scanner.getText()));
	scanner.advance();

	code->swap();
	code->method("__assign__", 1);
	code->pop();

	block(code);
	code->pop();

	code->patch(offset2);
}


//
//	OtCompiler::whileStatement
//

void OtCompiler::whileStatement(OtCode code) {
	scanner.expect(OtScanner::WHILE_TOKEN);
	size_t offset1 = code->size();

	if (expression(code)) {
		code->method("__deref__", 0);
	}

	size_t offset2 = code->size();
	code->jumpFalse(0);

	block(code);
	code->pop();
	code->jump(offset1);
	code->patch(offset2);
}


//
//	OtCompiler::statement
//

void OtCompiler::statement(OtCode code) {
	// mark start of statement
	code->mark(scanner.getTokenStart());

	// process statement
	switch (scanner.getToken()) {
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

		case OtScanner::THROW_TOKEN:
			throwStatement(code);
			code->push(nullptr);
			break;

		case OtScanner::TRY_TOKEN:
			tryStatement(code);
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

	// mark end of statement
	code->mark(scanner.getLastTokenEnd());

	while (scanner.matchToken(OtScanner::SEMICOLON_TOKEN)) {
		scanner.advance();
	}
}
