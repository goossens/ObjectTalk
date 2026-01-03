//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <cstdint>
#include <limits>
#include <string>
#include <unordered_map>
#include <vector>

#include "OtException.h"
#include "OtIdentifier.h"
#include "OtSource.h"


//
//	OtScanner
//

class OtScanner {
public:
	// basic tokens
	enum class Token {
		illegal,
		endOfText,

		identifier,

		integerLiteral,
		realLiteral,
		stringLiteral,

		leftParenthesis,
		rightParenthesis,
		leftBracket,
		rightBracket,
		leftBrace,
		rightBrace,

		comma,
		period,
		colon,
		semicolon,
		questionMark,

		assignment,
		multiplyAssignment,
		divideAssignment,
		moduloAssignment,
		addAssignment,
		subtractAssignment,
		shiftLeftAssignment,
		shiftRightAssignment,
		bitwiseAndAssignment,
		bitwiseOrAssignment,
		bitwiseXorAssignment,

		equalOperator,
		notEqualOperator,
		lessOperator,
		lessEqualOperator,
		greaterOperator,
		greaterEqualOperator,

		negateOperator,
		andOperator,
		orOperator,

		addOperator,
		subtractOperator,
		multiplyOperator,
		divideOperator,
		powerOperator,
		moduloOperator,

		incrementOperator,
		decrementOperator,

		bitwiseAndOperator,
		bitwiseOrOperator,
		bitwiseXorOperator,
		bitwiseNotOperator,

		shiftLeftOperator,
		shiftRightOperator,

		catchKeyword,
		classKeyword,
		doKeyword,
		elifKeyword,
		elseKeyword,
		forKeyword,
		functionKeyword,
		ifKeyword,
		inKeyword,
		notKeyword,
		returnKeyword,
		superKeyword,
		throwKeyword,
		tryKeyword,
		varKeyword,
		whileKeyword
	};

	// constructor
	OtScanner();

	// load source code to scan
	void loadSource(OtSource source);

	// advance the scanner by parsing the next token
	Token advance();

	// get current token information
	inline Token getToken() { return token; }
	inline bool matchToken(Token _token) { return token == _token; }
	inline size_t getTokenStart() { return tokenStart; }
	inline size_t getLastTokenEnd() { return lastTokenEnd; }
	inline std::string getText() { return source->substr(tokenStart, position - tokenStart); }
	inline int64_t getInteger() { return integerValue; }
	inline double getReal() { return realValue; }
	inline std::string getString() { return stringValue; }
	inline OtID getID() { return OtIdentifier::create(getText()); }

	// throw an exception
	void error(std::string message);

	// see if the current token is equal to the specified token
	// if not, raise an exception
	void expect(Token token, bool advanceToNextToken=true);

private:
	// specify a new token to the scanner
	void addToken(const std::string text, Token textToken);

	// state definition for token state/transition table
	class OtScannerState {
	public:
		// constructor
		inline OtScannerState() {
			for (auto c = 0; c < 256; c++) {
				transitions[c] = noTransition;
			}

			token = Token::illegal;
		}

		// state transitions
		size_t transitions[256];
		static constexpr size_t noTransition = std::numeric_limits<std::size_t>::max();

		// token for this state
		Token token;
	};

	// token lookup
	std::unordered_map<Token, std::string> tokens;

	// token state/transition table
	std::vector<OtScannerState> stateTable;

	// source code to be scanned
	OtSource source;

	// scanner state
	size_t size;
	size_t position;
	size_t tokenStart;
	size_t tokenEnd;
	size_t lastTokenStart;
	size_t lastTokenEnd;

	Token token;
	int64_t integerValue;
	double realValue;
	std::string stringValue;
};
