//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "fmt/format.h"

#include "OtException.h"
#include "OtScanner.h"
#include "OtText.h"


//
//	OtScanner::OtScanner
//

OtScanner::OtScanner() {
	// set default scanner state
	token = Token::illegal;
	stateTable.resize(1);

	// setup scanner
	addToken("(", Token::leftParenthesis);
	addToken(")", Token::rightParenthesis);
	addToken("[", Token::leftBracket);
	addToken("]", Token::rightBracket);
	addToken("{", Token::leftBrace);
	addToken("}", Token::rightBrace);
	addToken(",", Token::comma);
	addToken(".", Token::period);
	addToken(":", Token::colon);
	addToken(";", Token::colon);
	addToken("?", Token::questionMark);
	addToken("==", Token::equalOperator);
	addToken("=", Token::assignment);
	addToken("!=", Token::notEqualOperator);
	addToken("!", Token::negateOperator);
	addToken("<=", Token::lessEqualOperator);
	addToken("<<=", Token::shiftLeftAssignment);
	addToken("<<", Token::shiftLeftOperator);
	addToken("<", Token::lessOperator);
	addToken(">=", Token::greaterEqualOperator);
	addToken(">>=", Token::shiftRightAssignment);
	addToken(">>", Token::shiftRightOperator);
	addToken(">", Token::greaterOperator);
	addToken("++", Token::incrementOperator);
	addToken("+=", Token::addAssignment);
	addToken("+", Token::addOperator);
	addToken("--", Token::decrementOperator);
	addToken("-=", Token::subtractAssignment);
	addToken("-", Token::subtractOperator);
	addToken("*=", Token::multiplyAssignment);
	addToken("**", Token::powerOperator);
	addToken("*", Token::multiplyOperator);
	addToken("/=", Token::divideAssignment);
	addToken("/", Token::divideOperator);
	addToken("%=", Token::moduloAssignment);
	addToken("%", Token::moduloOperator);
	addToken("||", Token::orOperator);
	addToken("|=", Token::bitwiseOrAssignment);
	addToken("|", Token::bitwiseOrOperator);
	addToken("&&", Token::andOperator);
	addToken("&=", Token::bitwiseAndAssignment);
	addToken("&", Token::bitwiseAndOperator);
	addToken("^=", Token::bitwiseXorAssignment);
	addToken("^", Token::bitwiseXorOperator);
	addToken("~", Token::bitwiseNotOperator);
	addToken("catch", Token::catchKeyword);
	addToken("class", Token::classKeyword);
	addToken("do", Token::doKeyword);
	addToken("elif", Token::elifKeyword);
	addToken("else", Token::elseKeyword);
	addToken("for", Token::forKeyword);
	addToken("function", Token::functionKeyword);
	addToken("if", Token::ifKeyword);
	addToken("in", Token::inKeyword);
	addToken("not", Token::notKeyword);
	addToken("return", Token::returnKeyword);
	addToken("super", Token::superKeyword);
	addToken("throw", Token::throwKeyword);
	addToken("try", Token::tryKeyword);
	addToken("var", Token::varKeyword);
	addToken("while", Token::whileKeyword);
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
//	Helper functions
//

static inline bool isBinaryPrefix(char c) { return c == 'b' || c == 'B'; }
static inline bool isOctalPrefix(char c) { return c == 'o' || c == 'O'; }
static inline bool isHexPrefix(char c) { return c == 'x' || c == 'X'; }

static inline bool isBinaryDigit(char c) { return c == '0' || c == '1'; }
static inline bool isOctalDigit(char c) { return c >= '0' && c <= '7'; }


//
//	OtScanner::advance
//

OtScanner::Token OtScanner::advance() {
	// skip all whitespaces and comments
	while (isspace(source->at(position)) ||
		   (source->at(position) == '#') ||
		   (source->at(position) == '/' && source->at(position + 1) == '*') ||
		   (source->at(position) == '/' && source->at(position + 1) == '/')) {

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

		// skip C style comments
		} else if (source->at(position) =='/' && source->at(position + 1) =='*') {
			position += 2;

			while (position < size && !(source->at(position) =='*' && source->at(position + 1) =='/')) {
				position++;
			}

			if (position < size) {
				position += 2;
			}

		// skip C++ style comments
		} else if (source->at(position) =='/' && source->at(position + 1) =='/') {
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
		token = Token::endOfText;

	// handle numeric literals
	} else if (std::isdigit(source->at(position)) ||
		(source->at(position) =='-' && position < size && std::isdigit(source->at(position + 1))) ||
		(source->at(position) =='+' && position < size && std::isdigit(source->at(position + 1)))) {

		auto start = position;
		int sign = 1;

		if (source->at(position) =='-') {
			position++;
			sign = -1;

		} else if (source->at(position) =='+') {
			position++;
		}

		// see if we have a binary literal
		if (position + 3 < size && source->at(position) == '0' && isBinaryPrefix(source->at(position + 1)) && isBinaryDigit(source->at(position + 2))) {
			position += 2;
			auto value = position;

			while (position < size && isBinaryDigit(source->at(position))) {
				position++;
			}

			integerValue = sign * std::stol(source->substr(value, position - value), 0, 2);
			token = Token::integerLiteral;

		// see if we have an octal literal
		} else if (position + 3 < size && source->at(position) == '0' && isOctalPrefix(source->at(position + 1)) && isOctalDigit(source->at(position + 2))) {
			position += 2;
			auto value = position;

			while (position < size && isOctalDigit(source->at(position))) {
				position++;
			}

			integerValue = sign * std::stol(source->substr(value, position - value), 0, 8);
			token = Token::integerLiteral;

		// see if we have a hexadecimal literal
		} else if (position + 3 < size && source->at(position) == '0' && isHexPrefix(source->at(position + 1)) && isxdigit(source->at(position + 2))) {
			position += 2;
			auto value = position;

			while (position < size && isxdigit(source->at(position))) {
				position++;
			}

			integerValue = sign * std::stol(source->substr(value, position - value), 0, 16);
			token = Token::integerLiteral;

		// see if we have a C-style octal literal
		} else if (position + 2 < size && source->at(position) == '0' && isOctalDigit(source->at(position + 1))) {
			auto value = ++position;

			while (position < size && isOctalDigit(source->at(position))) {
				position++;
			}

			integerValue = sign * std::stol(source->substr(value, position - value), 0, 8);
			token = Token::integerLiteral;

		// handle decimal integers and reals
		} else {
			// handle integer part
			while (position < size && std::isdigit(source->at(position))) {
				position++;
			}

			// is this a real?
			if (position < size && source->at(position) =='.' && position < size && std::isdigit(source->at(position + 1))) {
				position++;

				while (position < size && std::isdigit(source->at(position))) {
					position++;
				}

				if (tolower(source->at(position)) =='e' && position < size) {
					position++;

					if (position < size && source->at(position) =='-') {
						position++;

					} else if (position < size && source->at(position) =='+') {
						position++;
					}

					while (position < size && std::isdigit(source->at(position))) {
						position++;
					}
				}

				realValue = std::stod(source->substr(start, position - start));
				token = Token::realLiteral;

			} else {
				integerValue = std::stoi(source->substr(start, position - start));
				token = Token::integerLiteral;
			}
		}

	// handle string literals
	} else if (source->at(position) =='"') {
		auto start = ++position;

		while (position < size && !(source->at(position) == '"' && !(source->at(position - 1) == '\\'))) {
			position++;
		}

		stringValue = OtText::fromJSON(source->substr(start, position - start));

		if (position < size) {
			position++;
		}

		token = Token::stringLiteral;

	// handle identifiers (and tokens with identifier structure)
	} else if (source->at(position) =='_' || std::isalpha(source->at(position))) {
		while (position < size && (source->at(position) =='_' || std::isalnum(source->at(position)))) {
			position++;
		}

		size_t state = 0;

		for (auto p = tokenStart; state != OtScannerState::noTransition && p < position; p++) {
			state = stateTable[state].transitions[static_cast<size_t>(source->at(p))];
		}

		if (state != OtScannerState::noTransition && stateTable[state].token != Token::illegal) {
			token = stateTable[state].token;

		} else {
			token = Token::identifier;
		}

	// handle (non-identifier) tokens
	} else {
		size_t state = 0;

		while (position < size && stateTable[state].transitions[static_cast<size_t>(source->at(position))] != OtScannerState::noTransition) {
			state = stateTable[state].transitions[static_cast<size_t>(source->at(position++))];
		}

		if (position > tokenStart && stateTable[state].token != Token::illegal) {
			token = stateTable[state].token;

		} else {
			// we tried but whatever we are looking at, it's illegal
			token = Token::illegal;
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
	auto moduleName = source->getModule();
	auto start = source->getStartOfLine(tokenStart);
	auto lineText = source->getLine(tokenStart);
	auto lineNumber = source->getLineNumber(tokenStart);
	std::string marker;

	for (auto c = 0; c < tokenStart - start; c++) {
		if (std::isspace(lineText[c])) {
			marker += lineText[c];

		} else {
			marker +=' ';
		}
	}

	marker +='^';
	auto fullMessage = fmt::format("Module: {}, Line: {}: {}:\n{}\n{}", moduleName, lineNumber, message, lineText, marker);

	// throw exception
	throw OtException(moduleName, lineNumber, tokenStart, tokenEnd, message, fullMessage);
}


//
//	OtScanner::expect
//

void OtScanner::expect(Token expectedToken, bool advanceToNextToken) {
	if (token == expectedToken) {
		if (advanceToNextToken) {
			advance();
		}

	} else {
		error(fmt::format("Expected [{}]", tokens[expectedToken]));
	}
}


//
//	OtScanner::addToken
//

void OtScanner::addToken(const std::string text, Token token) {
	// add token to lookup
	tokens[token] = text;

	// set start state
	short state = 0;

	// process all characters
	for (auto i = text.begin(); i < text.end(); i++) {
		// determine next state
		auto next = stateTable[state].transitions[static_cast<size_t>(*i)];

		// add a new state if required
		if (next == OtScannerState::noTransition) {
			next = stateTable.size();
			stateTable.resize(next + 1);
			stateTable[state].transitions[static_cast<size_t>(*i)] = next;
		}

		// set the next state
		state = next;
	}

	// store the token
	stateTable[state].token = token;
}
