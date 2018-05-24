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

#include <cctype>
#include <map>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

#include "OtException.h"


//
//  OtScanner
//

typedef int OtToken;

class OtScanner
{
public:
	// basic tokens
	enum
	{
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
		BITWISE_AND_ASSIGNMENT_TOKEN,
		BITWISE_OR_ASSIGNMENT_TOKEN,
		BITWISE_XOR_ASSIGNMENT_TOKEN,

		EQUAL_TOKEN,
		NOT_EQUAL_TOKEN,
		LESS_TOKEN,
		LESS_EQUAL_TOKEN,
		GREATER_TOKEN,
		GREATER_EQUAL_TOKEN,

		NOT_TOKEN,
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

		ELLIPSIS_TOKEN,

		CASE_TOKEN,
		CLASS_TOKEN,
		DEFAULT_TOKEN,
		DO_TOKEN,
		ELSE_TOKEN,
		FOR_TOKEN,
		FUNCTION_TOKEN,
		IF_TOKEN,
		RETURN_TOKEN,
		SWITCH_TOKEN,
		WHILE_TOKEN

	};

	// constructor
	OtScanner()
	{
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
		addToken("!", NOT_TOKEN);
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
		addToken("return", RETURN_TOKEN);
		addToken("switch", SWITCH_TOKEN);
		addToken("while", WHILE_TOKEN);
	}

	// specify a new token to the scanner
	void addToken(const std::string text, int token)
	{
		// add token to lookup
		tokens[token] = text;

		// set start state
		short state = 0;

		// process all characters
		for (auto i = text.begin(); i < text.end(); i++)
		{
			// determine next state
			size_t next = stateTable[state].transitions[(int) *i];

			// add a new state if required
			if (next == OtScannerState::noTransition)
			{
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

	// load text to scan
	void loadText(const std::string t)
	{
		// save text to be scanned
		text = t;
		size = text.length();

		// set scanner state
		position = 0;
		lineNumber = 1;

		// go to first token
		advance();
	}

	// advance the scanner by parsing the next token
	OtToken advance()
	{
		// skip all white space and comments
		while (isspace(text[position]) ||
			   (text[position] == '#') ||
			   (text[position] == '/' && text[position + 1] == '*') ||
			   (text[position] == '/' && text[position + 1] == '/'))
		{
			// skip white space
			while (isspace(text[position]))
			{
				if (text[position] == '\n')
					lineNumber++;

				position++;
			}

			// skip C style comments
			if (text[position] == '/' && text[position + 1] == '*')
			{
				position += 2;

				while (position < size && !(text[position] == '*' && text[position + 1] == '/'))
				{
					if (text[position] == '\n')
						lineNumber++;

					position++;
				}

				if (position < size)
					position += 2;
			}

			// skip C++ style comments
			else if (text[position] == '/' && text[position + 1] == '/')
			{
				position += 2;

				while (position < size && !(text[position + 1] == '\n'))
					position++;

				if (position < size)
				{
					position++;
					lineNumber++;
				}
			}

			// skip shell style comments
			else if (text[position] == '#')
			{
				position++;

				while (position < size && !(text[position + 1] == '\n'))
					position++;

				if (position < size)
				{
					position++;
					lineNumber++;
				}
			}
		}

		// save start of token
		tokenStart = position;
		tokenLine = lineNumber;

		// check for end of string
		if (position == size)
			token = EOS_TOKEN;

		// handle numerical values
		else if (std::isdigit(text[position]) || (text[position] == '-' && position < size && std::isdigit(text[position + 1])))
		{
			size_t start = position;

			if (text[position] == '-')
				position++;

			while (std::isdigit(text[position]))
				position++;

			if (text[position] == '.' && position < size && std::isdigit(text[position + 1]))
			{
				position++;

				while (std::isdigit(text[position]))
					position++;

				if (tolower(text[position]) == 'e' && position < size)
				{
					position++;

					while (std::isdigit(text[position]))
						position++;
				}

				realValue = std::stod(text.substr(start, position - start));
				token = REAL_TOKEN;
			}

			else
			{
				integerValue = std::stoi(text.substr(start, position - start));
				token = INTEGER_TOKEN;
			}
		}

		// handle strings
		else if (text[position] == '"' || text[position] == '\'')
		{
			char terminator = text[position++];
			stringValue = "";

			while (position < size && text[position] != terminator)
			{
				// is this a new line?
				if (text[position] == '\n')
					lineNumber++;

				if (text[position] == '\\')
				{
					position++;

					if (position < size)
					{
						char character = text[position++];

						switch (character)
						{
							case 'a':
								stringValue += '\a';
								break;

							case 'b':
								stringValue += '\b';
								break;

							case 'f':
								stringValue += '\f';
								break;

							case 'n':
								stringValue += '\n';
								break;

							case 'r':
								stringValue += '\r';
								break;

							case 't':
								stringValue += '\t';
								break;

							case 'v':
								stringValue += '\v';
								break;

							default:
								stringValue += character;
						}
					}
				}

				else
					stringValue += text[position++];
			}

			if (position < size)
				position++;

			token = STRING_TOKEN;
		}

		// handle identifiers (and tokens with identifier structure)
		else if (text[position] == '_' || std::isalpha(text[position]))
		{
			while (text[position] == '_' || std::isalnum(text[position]))
				position++;

			size_t state = 0;

			for (auto p = tokenStart; state != OtScannerState::noTransition && p < position; p++)
				state = stateTable[state].transitions[(int) text[p]];

			if (state != OtScannerState::noTransition && stateTable[state].token != ILLEGAL_TOKEN)
				token = stateTable[state].token;

			else
				token = IDENTIFIER_TOKEN;
		}

		// handle (non-identifier) tokens
		else
		{
			// see if we can find a token
			size_t state = 0;

			while(position < size && stateTable[state].transitions[(int) text[position]] != OtScannerState::noTransition)
				state = stateTable[state].transitions[(int) text[position++]];

			if (position > tokenStart && stateTable[state].token != ILLEGAL_TOKEN)
				token = stateTable[state].token;

			else
				// we tried but whatever we are looking at, it's illegal
				token = ILLEGAL_TOKEN;
		}

		// return the token we just scanned
		return token;
	}

	// get current token information
	OtToken getToken() { return token; }
	bool matchToken(OtToken _token) { return token == _token; }
	size_t getTokenStart() { return tokenStart; }
	std::string getText() { return text.substr(tokenStart, position - tokenStart); }
	std::string getTextFrom(size_t start) { return text.substr(start, tokenStart - start); }
	long getInteger() { return integerValue; }
	double getReal() { return realValue; }
	std::string getString() { return stringValue; }

	// throw an exection
	void error(std::string message)
	{
		// find start of line
		size_t start = tokenStart;

		while (start && text[start - 1] != '\n')
			start--;

		// find end of line
		size_t end = start;

		while (start < size && text[end] != '\n')
			end++;

		// extract line and create marker
		std::string line = text.substr(start, end - start);
		std::string marker;

		for (size_t c = 0; c < tokenStart - start; c++)
			if (std::isspace(line[c]))
				marker +=line[c];

			else
				marker += ' ';

		marker += '^';

		// throw exception
		OT_EXCEPT("%s on line %d:\n%s\n%s", message.c_str(), tokenLine, line.c_str(), marker.c_str());
	}

	// see if the current token is equal to the specified token
	// if not, raise an exception
	void expect(OtToken t, bool a=true)
	{
		if (token == t)
		{
			if (a)
				advance();
		}

		else
			error(OtFormat("Expected [%s]", tokens[t].c_str()));
	}

	// state definition for token state/transition table
	class OtScannerState
	{
	public:
		// constructor
		OtScannerState()
		{
			for (auto c = 0; c < 256; c++)
				transitions[c] = noTransition;

			token = ILLEGAL_TOKEN;
		}

		// state transitions
		size_t transitions[256];
		static const size_t noTransition = std::numeric_limits<std::size_t>::max();

		// token for this state
		OtToken token;
	};

	// token lookup
	std::map<OtToken, std::string> tokens;

	// token state/transition table
	std::vector<OtScannerState> stateTable;

	// text to be scanned
	std::string text;

	// scanner state
	size_t size;
	size_t position;
	size_t lineNumber;
	size_t tokenStart;
	size_t tokenLine;

	OtToken token;
	long integerValue;
	double realValue;
	std::string stringValue;
};
