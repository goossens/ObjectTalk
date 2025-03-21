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

	// skip Byte Order Mark (BOM) just in case there is one
	static std::string::const_iterator skipBOM(std::string::const_iterator i, std::string::const_iterator end);

	// read the next codepoint and update iterator
	static std::string::const_iterator read(std::string::const_iterator i, std::string::const_iterator end, char32_t* codepoint);

	// write codepoint in UTF-8 and update iterator
	static std::string::iterator write(std::string::iterator i, char32_t codepoint);

	// get information about codepoint
	static bool isAlphabetic(char32_t codepoint);
	static bool isNumeric(char32_t codepoint);
	static bool isWhiteSpace(char32_t codepoint);
	static bool isXidStart(char32_t codepoint);
	static bool isXidContinue(char32_t codepoint);
	static bool isLowerCase(char32_t codepoint);
	static bool isUpperCase(char32_t codepoint);

	// convert codepoint
	static char32_t toLowerCase(char32_t codepoint);
	static char32_t toUpperCase(char32_t codepoint);
};
