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
		OT_SCANNER_ILLEGAL_TOKEN = -1,
		OT_SCANNER_EOS_TOKEN = 1,

		OT_SCANNER_IDENTIFIER_TOKEN,
		OT_SCANNER_INTEGER_TOKEN,
		OT_SCANNER_REAL_TOKEN,
		OT_SCANNER_STRING_TOKEN,
		OT_SCANNER_MAX_TOKEN
	};

	// constructor
	OtScanner()
	{
		// set default scanner state
		token = OT_SCANNER_ILLEGAL_TOKEN;
		stateTable.resize(1);
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
			token = OT_SCANNER_EOS_TOKEN;

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
				token = OT_SCANNER_REAL_TOKEN;
			}

			else
			{
				integerValue = std::stoi(text.substr(start, position - start));
				token = OT_SCANNER_INTEGER_TOKEN;
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

			token = OT_SCANNER_STRING_TOKEN;
		}

		// handle identifiers (and tokens with identifier structure)
		else if (text[position] == '_' || std::isalpha(text[position]))
		{
			while (text[position] == '_' || std::isalnum(text[position]))
				position++;

			size_t state = 0;

			for (auto p = tokenStart; state != OtScannerState::noTransition && p < position; p++)
				state = stateTable[state].transitions[(int) text[p]];

			if (state != OtScannerState::noTransition && stateTable[state].token != OT_SCANNER_ILLEGAL_TOKEN)
				token = stateTable[state].token;

			else
				token = OT_SCANNER_IDENTIFIER_TOKEN;
		}

		// handle (non-identifier) tokens
		else
		{
			// see if we can find a token
			size_t state = 0;

			while(position < size && stateTable[state].transitions[(int) text[position]] != OtScannerState::noTransition)
				state = stateTable[state].transitions[(int) text[position++]];

			if (position > tokenStart && stateTable[state].token != OT_SCANNER_ILLEGAL_TOKEN)
				token = stateTable[state].token;

			else
				// we tried but whatever we are looking at, it's illegal
				token = OT_SCANNER_ILLEGAL_TOKEN;
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

			token = OT_SCANNER_ILLEGAL_TOKEN;
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
