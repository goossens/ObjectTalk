//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/scanner.h"

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
	addToken("...", ELLIPSIS_TOKEN);
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
	addToken("case", CASE_TOKEN);
	addToken("class", CLASS_TOKEN);
	addToken("default", DEFAULT_TOKEN);
	addToken("do", DO_TOKEN);
	addToken("else", ELSE_TOKEN);
	addToken("for", FOR_TOKEN);
	addToken("function", FUNCTION_TOKEN);
	addToken("if", IF_TOKEN);
	addToken("in", IN_TOKEN);
	addToken("not", NOT_TOKEN);
	addToken("return", RETURN_TOKEN);
	addToken("switch", SWITCH_TOKEN);
	addToken("while", WHILE_TOKEN);
}


//
//	OtScanner::loadText
//

void OtScanner::loadText(const std::string& t) {
	// save text to be scanned
	text = t;
	size = text.length();

	// set scanner state
	position = 0;
	lineNumber = 1;

	// go to first token
	advance();
}


//
//	OtScanner::advance
//

OtToken OtScanner::advance() {
	// skip all white space and comments
	while (isspace(text[position]) ||
		   (text[position] =='#') ||
		   (text[position] =='/' && text[position + 1] =='*') ||
		   (text[position] =='/' && text[position + 1] =='/')) {
		// skip white space
		while (isspace(text[position])) {
			if (text[position] =='\n')
				lineNumber++;

			position++;
		}

		// skip C style comments
		if (text[position] =='/' && text[position + 1] =='*') {
			position += 2;

			while (position < size && !(text[position] =='*' && text[position + 1] =='/')) {
				if (text[position] =='\n')
					lineNumber++;

				position++;
			}

			if (position < size) {
				position += 2;
			}
		}

		// skip C++ style comments
		else if (text[position] =='/' && text[position + 1] =='/') {
			position += 2;

			while (position < size && !(text[position] == '\n')) {
				position++;
			}

			if (position < size) {
				position++;
				lineNumber++;
			}

		} else if (text[position] =='#') {
		// skip shell style comments
			position++;

			while (position < size && !(text[position] == '\n')) {
				position++;
			}

			if (position < size) {
				position++;
				lineNumber++;
			}
		}
	}

	// save information on previous token
	lastTokenStart = tokenStart;
	lastTokenEnd = tokenEnd;

	// save start of token
	tokenStart = position;
	tokenLine = lineNumber;

	// check for end of string
	if (position == size) {
		token = EOS_TOKEN;

	} else if (std::isdigit(text[position]) || (text[position] =='-' && position < size && std::isdigit(text[position + 1]))) {
		// handle numerical values
		auto start = position;

		if (text[position] =='-') {
			position++;
		}

		while (std::isdigit(text[position])) {
			position++;
		}

		if (text[position] =='.' && position < size && std::isdigit(text[position + 1])) {
			position++;

			while (std::isdigit(text[position])) {
				position++;
			}

			if (tolower(text[position]) =='e' && position < size) {
				position++;

				while (std::isdigit(text[position])) {
					position++;
				}
			}

			realValue = std::stod(text.substr(start, position - start));
			token = REAL_TOKEN;

		} else {
			integerValue = std::stoi(text.substr(start, position - start));
			token = INTEGER_TOKEN;
		}

	} else if (text[position] =='"') {
		// handle strings
		auto start = ++position;

		while (position < size && text[position] != '"') {
			if (text[position] =='\n') {
				lineNumber++;
			}

			position++;
		}

		stringValue = OtTextFromJSON(text.substr(start, position - start));

		if (position < size) {
			position++;
		}

		token = STRING_TOKEN;

	} else if (text[position] =='_' || std::isalpha(text[position])) {
		// handle identifiers (and tokens with identifier structure)
		while (text[position] =='_' || std::isalnum(text[position])) {
			position++;
		}

		size_t state = 0;

		for (auto p = tokenStart; state != OtScannerState::noTransition && p < position; p++) {
			state = stateTable[state].transitions[(int) text[p]];
		}

		if (state != OtScannerState::noTransition && stateTable[state].token != ILLEGAL_TOKEN) {
			token = stateTable[state].token;

		} else {
			token = IDENTIFIER_TOKEN;
		}

	} else {
		// handle (non-identifier) tokens
		size_t state = 0;

		while (position < size && stateTable[state].transitions[(int) text[position]] != OtScannerState::noTransition) {
			state = stateTable[state].transitions[(int) text[position++]];
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
	// find start of line
	auto start = tokenStart;

	while (start && text[start - 1] !='\n') {
		start--;
	}

	// find end of line
	auto end = start;

	while (start < size && text[end] !='\n') {
		end++;
	}

	// extract line and create marker
	auto line = text.substr(start, end - start);
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
	OT_EXCEPT("%s on line %d:\n%s\n%s", message.c_str(), tokenLine, line.c_str(), marker.c_str());
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
