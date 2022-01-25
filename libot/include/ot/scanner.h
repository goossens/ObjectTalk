//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>
#include <unordered_map>
#include <vector>

#include "exception.h"
#include "source.h"


//
//	OtScannerException
//

class OtScannerException : public OtException {
public:
	OtScannerException(
		const std::string& m,
		size_t l,
		size_t s,
	 	size_t e,
		const std::string& sm,
		const std::string& fm) :
			module(m),
			lineNumber(l),
			start(s),
			end(e),
			shortMessage(sm),
			OtException(fm) {
	}

	// access properties
	std::string getModule() const { return module; }
	size_t getLineNumber() const { return lineNumber; }
	size_t getStart() const { return start; }
	size_t getEnd() const { return end; }
	std::string getShortErrorMessage() const { return shortMessage; }

private:
	// the module in which the error happened
	std::string module;

	// line number causing the error (starting at 1)
	size_t lineNumber;

	// start of token causing the error (in bytes from start of source)
	size_t start;

	// end of token causing the error (in bytes from start of source)
	size_t end;

	// the short error message
	std::string shortMessage;
};


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
	OtToken getToken() { return token; }
	bool matchToken(OtToken _token) { return token == _token; }
	size_t getTokenStart() { return tokenStart; }
	size_t getLastTokenEnd() { return lastTokenEnd; }
	std::string getText() { return source->substr(tokenStart, position - tokenStart); }
	long getInteger() { return integerValue; }
	double getReal() { return realValue; }
	std::string getString() { return stringValue; }

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
	long integerValue;
	double realValue;
	std::string stringValue;
};
