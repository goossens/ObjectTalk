//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/scanner.h"

#include "ot/format.h"
#include "ot/exception.h"
#include "ot/text.h"


//
//	OtScanner::OtScanner
//

OtScanner::OtScanner() {
	// set default scanner state
	token = ILLEGAL_TOKEN;
	stateTable.resize(1);

	// setup scanner
	addToken("(", LPAREN_TOKEN);
	addToken(")", RPAREN_TOKEN);
	addToken("[", LBRACKET_TOKEN);
	addToken("]", RBRACKET_TOKEN);
	addToken("{", LBRACE_TOKEN);
	addToken("}", RBRACE_TOKEN);
	addToken(",", COMMA_TOKEN);
	addToken(".", PERIOD_TOKEN);
	addToken(":", COLON_TOKEN);
	addToken(";", SEMICOLON_TOKEN);
	addToken("?", QUESTION_TOKEN);
	addToken("==", EQUAL_TOKEN);
	addToken("=", ASSIGNMENT_TOKEN);
	addToken("!=", NOT_EQUAL_TOKEN);
	addToken("!", NEGATE_TOKEN);
	addToken("<=", LESS_EQUAL_TOKEN);
	addToken("<<", SHIFT_LEFT_TOKEN);
	addToken("<", LESS_TOKEN);
	addToken(">=", GREATER_EQUAL_TOKEN);
	addToken(">>", SHIFT_RIGHT_TOKEN);
	addToken(">", GREATER_TOKEN);
	addToken("++", INCREMENT_TOKEN);
	addToken("+=", ADD_ASSIGNMENT_TOKEN);
	addToken("+", ADD_TOKEN);
	addToken("--", DECREMENT_TOKEN);
	addToken("-=", SUBTRACT_ASSIGNMENT_TOKEN);
	addToken("-", SUBTRACT_TOKEN);
	addToken("*=", MULTIPLY_ASSIGNMENT_TOKEN);
	addToken("**", POWER_TOKEN);
	addToken("*", MULTIPLY_TOKEN);
	addToken("/=", DIVIDE_ASSIGNMENT_TOKEN);
	addToken("/", DIVIDE_TOKEN);
	addToken("%=", MODULO_ASSIGNMENT_TOKEN);
	addToken("%", MODULO_TOKEN);
	addToken("||", OR_TOKEN);
	addToken("|=", BITWISE_AND_ASSIGNMENT_TOKEN);
	addToken("|", BITWISE_OR_TOKEN);
	addToken("&&", AND_TOKEN);
	addToken("&=", BITWISE_OR_ASSIGNMENT_TOKEN);
	addToken("&", BITWISE_AND_TOKEN);
	addToken("^=", BITWISE_XOR_ASSIGNMENT_TOKEN);
	addToken("^", BITWISE_XOR_TOKEN);
	addToken("~", BITWISE_NOT_TOKEN);
	addToken("catch", CATCH_TOKEN);
	addToken("class", CLASS_TOKEN);
	addToken("do", DO_TOKEN);
	addToken("elif", ELIF_TOKEN);
	addToken("else", ELSE_TOKEN);
	addToken("for", FOR_TOKEN);
	addToken("function", FUNCTION_TOKEN);
	addToken("if", IF_TOKEN);
	addToken("in", IN_TOKEN);
	addToken("not", NOT_TOKEN);
	addToken("return", RETURN_TOKEN);
	addToken("throw", THROW_TOKEN);
	addToken("try", TRY_TOKEN);
	addToken("var", VAR_TOKEN);
	addToken("while", WHILE_TOKEN);
}


//
//	OtScanner::loadSource
//

void OtScanner::loadSource(OtSource src) {
	// save text to be scanned
	source = src;
	size = src->size();

	// set scanner state
	position = 0;

	// go to first token
	advance();
}


//
//	OtScanner::advance
//

OtToken OtScanner::advance() {
	// skip all white space and comments
	while (isspace(source->at(position)) ||
		   (source->at(position) == '#') ||
		   (source->at(position) == '/' && source->at(position + 1) == '*') ||
		   (source->at(position) == '/' && source->at(position + 1) == '/')) {
		// skip white space
		while (isspace(source->at(position))) {
			position++;
		}

		// skip shell style comments
		if (source->at(position) =='#') {
			position++;

			while (position < size && !(source->at(position) == '\n')) {
				position++;
			}

			if (position < size) {
				position++;
			}

		} else if (source->at(position) =='/' && source->at(position + 1) =='*') {
			// skip C style comments
			position += 2;

			while (position < size && !(source->at(position) =='*' && source->at(position + 1) =='/')) {
				position++;
			}

			if (position < size) {
				position += 2;
			}

		} else if (source->at(position) =='/' && source->at(position + 1) =='/') {
			// skip C++ style comments
			position += 2;

			while (position < size && !(source->at(position) == '\n')) {
				position++;
			}

			if (position < size) {
				position++;
			}

		}
	}

	// save information on previous token
	lastTokenStart = tokenStart;
	lastTokenEnd = tokenEnd;

	// save start of token
	tokenStart = position;

	// check for end of string
	if (position == size) {
		token = EOS_TOKEN;

	// handle numerical values
	} else if (std::isdigit(source->at(position)) || (source->at(position) =='-' && position < size && std::isdigit(source->at(position + 1)))) {
		auto start = position;
		int sign = 1;

		if (source->at(position) =='-') {
			position++;
			sign = -1;
		}

		// see if we have a binary constant
		if (source->at(position) == '0' && (source->at(position + 1) == 'b' || source->at(position + 1) == 'B')) {
			position += 2;
			auto value = position;

			while (source->at(position) == '0' || source->at(position) == '1') {
				position++;
			}

			integerValue = sign * std::stol(source->substr(value, position - value), 0, 2);
			token = INTEGER_TOKEN;

		// see if we have an octal constant
		} else if (source->at(position) == '0' && (source->at(position + 1) == 'o' || source->at(position + 1) == 'O')) {
			position += 2;
			auto value = position;

			while (source->at(position) >= '0' && source->at(position) <= '7') {
				position++;
			}

			integerValue = sign * std::stol(source->substr(value, position - value), 0, 8);
			token = INTEGER_TOKEN;

		// see if we have a hexadecimal constant
		} else if (source->at(position) == '0' && (source->at(position + 1) == 'x' || source->at(position + 1) == 'X')) {
			position += 2;
			auto value = position;

			while (isxdigit(source->at(position))) {
				position++;
			}

			integerValue = sign * std::stol(source->substr(value, position - value), 0, 16);
			token = INTEGER_TOKEN;

		// handle integers and reals
		} else {
			while (std::isdigit(source->at(position))) {
				position++;
			}

			if (source->at(position) =='.' && position < size && std::isdigit(source->at(position + 1))) {
				position++;

				while (std::isdigit(source->at(position))) {
					position++;
				}

				if (tolower(source->at(position)) =='e' && position < size) {
					position++;

					while (std::isdigit(source->at(position))) {
						position++;
					}
				}

				realValue = std::stod(source->substr(start, position - start));
				token = REAL_TOKEN;

			} else {
				integerValue = std::stoi(source->substr(start, position - start));
				token = INTEGER_TOKEN;
			}
		}

	// handle strings
	} else if (source->at(position) =='"') {
		auto start = ++position;

		while (position < size && !(source->at(position) == '"' && !(source->at(position - 1) == '\\'))) {
			position++;
		}

		stringValue = OtText::fromJSON(source->substr(start, position - start));

		if (position < size) {
			position++;
		}

		token = STRING_TOKEN;

	// handle identifiers (and tokens with identifier structure)
	} else if (source->at(position) =='_' || std::isalpha(source->at(position))) {
		while (source->at(position) =='_' || std::isalnum(source->at(position))) {
			position++;
		}

		size_t state = 0;

		for (auto p = tokenStart; state != OtScannerState::noTransition && p < position; p++) {
			state = stateTable[state].transitions[(int) source->at(p)];
		}

		if (state != OtScannerState::noTransition && stateTable[state].token != ILLEGAL_TOKEN) {
			token = stateTable[state].token;

		} else {
			token = IDENTIFIER_TOKEN;
		}

	// handle (non-identifier) tokens
	} else {
		size_t state = 0;

		while (position < size && stateTable[state].transitions[(int) source->at(position)] != OtScannerState::noTransition) {
			state = stateTable[state].transitions[(int) source->at(position++)];
		}

		if (position > tokenStart && stateTable[state].token != ILLEGAL_TOKEN) {
			token = stateTable[state].token;

		} else {
			// we tried but whatever we are looking at, it's illegal
			token = ILLEGAL_TOKEN;
		}
	}

	// save end of token
	tokenEnd = position;

	// return the token we just scanned
	return token;
}


//
//	OtScanner::error
//

void OtScanner::error(std::string message) {
	// extract line and create marker
	auto start = source->getStartOfLine(tokenStart);
	auto line = source->getLine(tokenStart);
	std::string marker;

	for (auto c = 0; c < tokenStart - start; c++) {
		if (std::isspace(line[c])) {
			marker += line[c];

		} else {
			marker +=' ';
		}
	}

	marker +='^';

	// throw exception
	OtExcept("Module: %s, Line %ld: %s:\n%s\n%s",
		source->getModule().c_str(),
		source->getLineNumber(tokenStart),
		message.c_str(),
		line.c_str(),
		marker.c_str());
}


//
//	OtScanner::expect
//

void OtScanner::expect(OtToken t, bool a) {
	if (token == t) {
		if (a) {
			advance();
		}

	} else {
		error(OtFormat("Expected [%s]", tokens[t].c_str()));
	}
}


//
//	OtScanner::addToken
//

void OtScanner::addToken(const std::string text, int token) {
	// add token to lookup
	tokens[token] = text;

	// set start state
	short state = 0;

	// process all characters
	for (auto i = text.begin(); i < text.end(); i++) {
		// determine next state
		auto next = stateTable[state].transitions[(int) *i];

		// add a new state if required
		if (next == OtScannerState::noTransition) {
			next = stateTable.size();
			stateTable.resize(next + 1);
			stateTable[state].transitions[(int) *i] = next;
		}

		// set the next state
		state = next;
	}

	// store the token
	stateTable[state].token = token;
}
