//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtCodePoint.h"
#include "OtException.h"
#include "OtUnicode.h"

//
//	OtCodePointSize
//

size_t OtCodePoint::size(std::string::const_iterator i) {
	if ((*i & 0x80) == 0) {
		return 1;

	} else if ((*i & 0xE0) == 0xC0) {
		return 2;

	} else if ((*i & 0xF0) == 0xE0) {
		return 3;

	} else if ((*i & 0xF8) == 0xF0) {
		return 4;

	} else {
		OtError("Invalid codepoint in UTF-8 string");
	}

	return 0;
}


//
//	OtCodePointGet
//

std::string::const_iterator OtCodePoint::get(std::string::const_iterator i, char32_t* codepoint) {
	if ((*i & 0x80) == 0) {
		*codepoint = *i;
		i++;

	} else if ((*i & 0xE0) == 0xC0) {
		*codepoint = ((*i & 0x1f) << 6) | (*(i + 1) & 0x3f);
		i += 2;

	} else if ((*i & 0xF0) == 0xE0) {
		*codepoint = ((*i & 0x0f) << 12) | ((*(i + 1) & 0x3f) << 6) | (*(i + 2) & 0x3f);
		i += 3;

	} else if ((*i & 0xF8) == 0xF0) {
		*codepoint = ((*i & 0x07) << 18) | ((*(i + 1) & 0x3f) << 12) | ((*(i + 2) & 0x3f) << 6) | (*(i + 3) & 0x3f);
		i += 4;

	} else {
		OtError("Invalid codepoint in UTF-8 string");
	}

	return i;
}


//
//	OtCodePointPut
//

std::string::iterator OtCodePoint::put(std::string::iterator i, char32_t codepoint) {
	if ((codepoint & 0xffffff80) == 0) {
		*i = codepoint;
		i++;

	} else if ((codepoint & 0xffffff80) == 0) {
		*i = 0xc0 | ((codepoint >> 6) & 0x1f);
		*(i + 1) = 0x80 | (codepoint & 0x3f);
		i += 2;

	} else if ((codepoint & 0xffff0000) == 0) {
		*i = 0xe0 | ((codepoint >> 12) & 0x0f);
		*(i + 1) = 0x80 | ((codepoint >> 6) & 0x3f);
		*(i + 2) = 0x80 | (codepoint & 0x3f);
		i += 3;

	} else {
		*i = 0xf0 | ((codepoint >> 18) & 0x07);
		*(i + 1) = 0x80 | ((codepoint >> 12) & 0x3f);
		*(i + 2) = 0x80 | ((codepoint >> 6) & 0x3f);
		*(i + 3) = 0x80 | (codepoint & 0x3f);
		i += 4;
	}

	return i;
}


//
// 	Get information about codepoint
//

bool OtCodePoint::isPunctuation(char32_t codepoint) { return is_punctuation(codepoint); }
bool OtCodePoint::isHexDigit(char32_t codepoint) { return is_hex_digit(codepoint); }
bool OtCodePoint::isAlphabetic(char32_t codepoint) { return is_alphabetic(codepoint); }
bool OtCodePoint::isAlphaNumeric(char32_t codepoint) { return is_alphanumeric(codepoint); }
bool OtCodePoint::isUppercase(char32_t codepoint) { return is_uppercase(codepoint); }
bool OtCodePoint::isLowercase(char32_t codepoint) { return is_lowercase(codepoint); }
bool OtCodePoint::isWhiteSpace(char32_t codepoint) { return is_white_space(codepoint); }
bool OtCodePoint::isBlank(char32_t codepoint) { return is_blank(codepoint); }
bool OtCodePoint::isGraph(char32_t codepoint) { return is_graph(codepoint); }
bool OtCodePoint::isPrint(char32_t codepoint) { return is_print(codepoint); }


//
//	Convert codepoint
//

char32_t OtCodePoint::lowercase(char32_t codepoint) { return to_lowercase(codepoint); }
char32_t OtCodePoint::uppercase(char32_t codepoint) { return to_uppercase(codepoint); }
