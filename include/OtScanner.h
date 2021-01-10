//	ObjectTalk Scripting Language
//	Copyright 1993-2021 Johan A. Goossens
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

		ELLIPSIS_TOKEN,

		CASE_TOKEN,
		CLASS_TOKEN,
		DEFAULT_TOKEN,
		DO_TOKEN,
		ELSE_TOKEN,
		FOR_TOKEN,
		FUNCTION_TOKEN,
		IF_TOKEN,
		IN_TOKEN,
		NOT_TOKEN,
		RETURN_TOKEN,
		SWITCH_TOKEN,
		WHILE_TOKEN
	};

	// constructor
	OtScanner() {
		// set default scanner state
		token = ILLEGAL_TOKEN;
		stateTable.resize(1);

		// setup scanner
		addToken(L"(", LPAREN_TOKEN);
		addToken(L")", RPAREN_TOKEN);
		addToken(L"[", LBRACKET_TOKEN);
		addToken(L"]", RBRACKET_TOKEN);
		addToken(L"{", LBRACE_TOKEN);
		addToken(L"}", RBRACE_TOKEN);
		addToken(L",", COMMA_TOKEN);
		addToken(L"...", ELLIPSIS_TOKEN);
		addToken(L".", PERIOD_TOKEN);
		addToken(L":", COLON_TOKEN);
		addToken(L";", SEMICOLON_TOKEN);
		addToken(L"?", QUESTION_TOKEN);
		addToken(L"==", EQUAL_TOKEN);
		addToken(L"=", ASSIGNMENT_TOKEN);
		addToken(L"!=", NOT_EQUAL_TOKEN);
		addToken(L"!", NEGATE_TOKEN);
		addToken(L"<=", LESS_EQUAL_TOKEN);
		addToken(L"<<", SHIFT_LEFT_TOKEN);
		addToken(L"<", LESS_TOKEN);
		addToken(L">=", GREATER_EQUAL_TOKEN);
		addToken(L">>", SHIFT_RIGHT_TOKEN);
		addToken(L">", GREATER_TOKEN);
		addToken(L"++", INCREMENT_TOKEN);
		addToken(L"+=", ADD_ASSIGNMENT_TOKEN);
		addToken(L"+", ADD_TOKEN);
		addToken(L"--", DECREMENT_TOKEN);
		addToken(L"-=", SUBTRACT_ASSIGNMENT_TOKEN);
		addToken(L"-", SUBTRACT_TOKEN);
		addToken(L"*=", MULTIPLY_ASSIGNMENT_TOKEN);
		addToken(L"**", POWER_TOKEN);
		addToken(L"*", MULTIPLY_TOKEN);
		addToken(L"/=", DIVIDE_ASSIGNMENT_TOKEN);
		addToken(L"/", DIVIDE_TOKEN);
		addToken(L"%=", MODULO_ASSIGNMENT_TOKEN);
		addToken(L"%", MODULO_TOKEN);
		addToken(L"||", OR_TOKEN);
		addToken(L"|=", BITWISE_AND_ASSIGNMENT_TOKEN);
		addToken(L"|", BITWISE_OR_TOKEN);
		addToken(L"&&", AND_TOKEN);
		addToken(L"&=", BITWISE_OR_ASSIGNMENT_TOKEN);
		addToken(L"&", BITWISE_AND_TOKEN);
		addToken(L"^=", BITWISE_XOR_ASSIGNMENT_TOKEN);
		addToken(L"^", BITWISE_XOR_TOKEN);
		addToken(L"~", BITWISE_NOT_TOKEN);
		addToken(L"case", CASE_TOKEN);
		addToken(L"class", CLASS_TOKEN);
		addToken(L"default", DEFAULT_TOKEN);
		addToken(L"do", DO_TOKEN);
		addToken(L"else", ELSE_TOKEN);
		addToken(L"for", FOR_TOKEN);
		addToken(L"function", FUNCTION_TOKEN);
		addToken(L"if", IF_TOKEN);
		addToken(L"in", IN_TOKEN);
		addToken(L"not", NOT_TOKEN);
		addToken(L"return", RETURN_TOKEN);
		addToken(L"switch", SWITCH_TOKEN);
		addToken(L"while", WHILE_TOKEN);
	}

	// specify a new token to the scanner
	void addToken(const std::wstring text, int token) {
		// add token to lookup
		tokens[token] = text;

		// set start state
		short state = 0;

		// process all characters
		for (auto i = text.begin(); i < text.end(); i++) {
			// determine next state
			size_t next = stateTable[state].transitions[(int) *i];

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

	// load text to scan
	void loadText(const std::wstring t) {
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
	OtToken advance() {
		// skip all white space and comments
		while (isspace(text[position]) ||
			   (text[position] == L'#') ||
			   (text[position] == L'/' && text[position + 1] == L'*') ||
			   (text[position] == L'/' && text[position + 1] == L'/')) {
			// skip white space
			while (isspace(text[position])) {
				if (text[position] == L'\n')
					lineNumber++;

				position++;
			}

			// skip C style comments
			if (text[position] == L'/' && text[position + 1] == L'*') {
				position += 2;

				while (position < size && !(text[position] == L'*' && text[position + 1] == L'/')) {
					if (text[position] == L'\n')
						lineNumber++;

					position++;
				}

				if (position < size) {
					position += 2;
				}
			}

			// skip C++ style comments
			else if (text[position] == L'/' && text[position + 1] == L'/') {
				position += 2;

				while (position < size && !(text[position + 1] == L'\n')) {
					position++;
				}

				if (position < size) {
					position++;
					lineNumber++;
				}

			} else if (text[position] == L'#') {
			// skip shell style comments
				position++;

				while (position < size && !(text[position + 1] == L'\n')) {
					position++;
				}

				if (position < size) {
					position++;
					lineNumber++;
				}
			}
		}

		// save start of token
		tokenStart = position;
		tokenLine = lineNumber;

		// check for end of string
		if (position == size) {
			token = EOS_TOKEN;

		} else if (std::isdigit(text[position]) || (text[position] == L'-' && position < size && std::isdigit(text[position + 1]))) {
			// handle numerical values
			size_t start = position;

			if (text[position] == L'-') {
				position++;
			}

			while (std::isdigit(text[position])) {
				position++;
			}

			if (text[position] == L'.' && position < size && std::isdigit(text[position + 1])) {
				position++;

				while (std::isdigit(text[position])) {
					position++;
				}

				if (tolower(text[position]) == L'e' && position < size) {
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

		} else if (text[position] == L'"' || text[position] == L'\'') {
			// handle strings
			wchar_t terminator = text[position++];
			size_t start = position;

			while (position < size && text[position] != terminator) {
				if (text[position] == L'\n') {
					lineNumber++;
				}

				position++;
			}

			stringValue = OtTextFromJSON(text.substr(start, position - start));

			if (position < size) {
				position++;
			}

			token = STRING_TOKEN;

		} else if (text[position] == L'_' || std::isalpha(text[position])) {
			// handle identifiers (and tokens with identifier structure)
			while (text[position] == L'_' || std::isalnum(text[position])) {
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

		// return the token we just scanned
		return token;
	}

	// get current token information
	OtToken getToken() { return token; }
	bool matchToken(OtToken _token) { return token == _token; }
	size_t getTokenStart() { return tokenStart; }
	std::wstring getText() { return text.substr(tokenStart, position - tokenStart); }
	std::wstring getTextFrom(size_t start) { return text.substr(start, tokenStart - start); }
	long getInteger() { return integerValue; }
	double getReal() { return realValue; }
	std::wstring getString() { return stringValue; }

	// throw an exection
	void error(std::wstring message) {
		// find start of line
		size_t start = tokenStart;

		while (start && text[start - 1] != L'\n') {
			start--;
		}

		// find end of line
		size_t end = start;

		while (start < size && text[end] != L'\n') {
			end++;
		}

		// extract line and create marker
		std::wstring line = text.substr(start, end - start);
		std::wstring marker;

		for (size_t c = 0; c < tokenStart - start; c++) {
			if (std::isspace(line[c])) {
				marker += line[c];

			} else {
				marker += L' ';
			}
		}

		marker += L'^';

		// throw exception
		OT_EXCEPT(L"%ls on line %d:\n%ls\n%ls", message.c_str(), tokenLine, line.c_str(), marker.c_str());
	}

	// see if the current token is equal to the specified token
	// if not, raise an exception
	void expect(OtToken t, bool a=true) {
		if (token == t) {
			if (a) {
				advance();
			}

		} else {
			error(OtFormat(L"Expected [%ls]", tokens[t].c_str()));
		}
	}

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
	std::map<OtToken, std::wstring> tokens;

	// token state/transition table
	std::vector<OtScannerState> stateTable;

	// text to be scanned
	std::wstring text;

	// scanner state
	size_t size;
	size_t position;
	size_t lineNumber;
	size_t tokenStart;
	size_t tokenLine;

	OtToken token;
	long integerValue;
	double realValue;
	std::wstring stringValue;
};
