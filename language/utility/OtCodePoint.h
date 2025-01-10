//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>


//
//	OtCodepoint
//

class OtCodePoint {
public:
	// get the number of bytes required for this codepoint
	static size_t size(std::string::const_iterator i);

	// get the next codepoint and update iterator
	static std::string::const_iterator get(std::string::const_iterator i, char32_t* codepoint);

	// write codepoint in UTF-8 and update iterator
	static std::string::iterator put(std::string::iterator i, char32_t codepoint);

	// get information about codepoint
	static bool isPunctuation(char32_t codepoint);
	static bool isDecimalNumber(char32_t codepoint);
	static bool isHexDigit(char32_t codepoint);
	static bool isAlphabetic(char32_t codepoint);
	static bool isAlphaNumeric(char32_t codepoint);
	static bool isUppercase(char32_t codepoint);
	static bool isLowercase(char32_t codepoint);
	static bool isWhiteSpace(char32_t codepoint);
	static bool isBlank(char32_t codepoint);
	static bool isGraph(char32_t codepoint);
	static bool isPrint(char32_t codepoint);

	// convert codepoint
	static char32_t lowercase(char32_t codepoint);
	static char32_t uppercase(char32_t codepoint);
};
