//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
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

typedef int OtToken;

class OtScanner {
public:
	// basic tokens
	enum {
		ILLEGAL_TOKEN = -1,
		EOS_TOKEN = 1,

		IDENTIFIER_TOKEN,
		INTEGER_TOKEN,
		REAL_TOKEN,
		STRING_TOKEN,

		LPAREN_TOKEN,
		RPAREN_TOKEN,
		LBRACKET_TOKEN,
		RBRACKET_TOKEN,
		LBRACE_TOKEN,
		RBRACE_TOKEN,

		COMMA_TOKEN,
		PERIOD_TOKEN,
		COLON_TOKEN,
		SEMICOLON_TOKEN,
		QUESTION_TOKEN,

		ASSIGNMENT_TOKEN,
		MULTIPLY_ASSIGNMENT_TOKEN,
		DIVIDE_ASSIGNMENT_TOKEN,
		MODULO_ASSIGNMENT_TOKEN,
		ADD_ASSIGNMENT_TOKEN,
		SUBTRACT_ASSIGNMENT_TOKEN,
		SHIFT_LEFT_ASSIGNMENT_TOKEN,
		SHIFT_RIGHT_ASSIGNMENT_TOKEN,
		BITWISE_AND_ASSIGNMENT_TOKEN,
		BITWISE_OR_ASSIGNMENT_TOKEN,
		BITWISE_XOR_ASSIGNMENT_TOKEN,

		EQUAL_TOKEN,
		NOT_EQUAL_TOKEN,
		LESS_TOKEN,
		LESS_EQUAL_TOKEN,
		GREATER_TOKEN,
		GREATER_EQUAL_TOKEN,

		NEGATE_TOKEN,
		AND_TOKEN,
		OR_TOKEN,

		ADD_TOKEN,
		SUBTRACT_TOKEN,
		MULTIPLY_TOKEN,
		DIVIDE_TOKEN,
		POWER_TOKEN,
		MODULO_TOKEN,

		INCREMENT_TOKEN,
		DECREMENT_TOKEN,

		BITWISE_AND_TOKEN,
		BITWISE_NOT_TOKEN,
		BITWISE_OR_TOKEN,
		BITWISE_XOR_TOKEN,

		SHIFT_LEFT_TOKEN,
		SHIFT_RIGHT_TOKEN,

		CATCH_TOKEN,
		CLASS_TOKEN,
		DO_TOKEN,
		ELIF_TOKEN,
		ELSE_TOKEN,
		FOR_TOKEN,
		FUNCTION_TOKEN,
		IF_TOKEN,
		IN_TOKEN,
		NOT_TOKEN,
		RETURN_TOKEN,
		SUPER_TOKEN,
		THROW_TOKEN,
		TRY_TOKEN,
		VAR_TOKEN,
		WHILE_TOKEN
	};

	// constructor
	OtScanner();

	// load source code to scan
	void loadSource(OtSource source);

	// advance the scanner by parsing the next token
	OtToken advance();

	// get current token information
	inline OtToken getToken() { return token; }
	inline bool matchToken(OtToken _token) { return token == _token; }
	inline size_t getTokenStart() { return tokenStart; }
	inline size_t getLastTokenEnd() { return lastTokenEnd; }
	inline std::string getText() { return source->substr(tokenStart, position - tokenStart); }
	inline int64_t getInteger() { return integerValue; }
	inline double getReal() { return realValue; }
	inline std::string getString() { return stringValue; }
	inline size_t getID() { return OtIdentifier::create(getText()); }

	// throw an exection
	void error(std::string message);

	// see if the current token is equal to the specified token
	// if not, raise an exception
	void expect(OtToken token, bool advance=true);

private:
	// specify a new token to the scanner
	void addToken(const std::string text, int token);

	// state definition for token state/transition table
	class OtScannerState {
	public:
		// constructor
		OtScannerState() {
			for (auto c = 0; c < 256; c++) {
				transitions[c] = noTransition;
			}

			token = ILLEGAL_TOKEN;
		}

		// state transitions
		size_t transitions[256];
		static const size_t noTransition = std::numeric_limits<std::size_t>::max();

		// token for this state
		OtToken token;
	};

	// token lookup
	std::unordered_map<OtToken, std::string> tokens;

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

	OtToken token;
	int64_t integerValue;
	double realValue;
	std::string stringValue;
};
