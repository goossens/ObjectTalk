//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


// 	Fast ASCII ctype functions borrowed from the musl library
//	https://git.musl-libc.org/cgit/musl/tree/include/ctype.h


//
//	Include files
//

#include "OtCodePoint.h"
#include "OtException.h"
#include "OtUnicode.h"


//
//	Internal type conversions because "char" (in std::string) is signed
//

static constexpr char32_t invalidCodePoint = 0xFFFD;

static inline char32_t uch(char c) {
	return static_cast<char32_t>(c);
}

static inline char sch(char32_t i) {
	return static_cast<char>(i);
}


//
//	OtCodePoint::size
//

size_t OtCodePoint::size(std::string::const_iterator i) {
	if ((uch(*i) & 0x80) == 0) {
		return 1;

	} else if ((uch(*i) & 0xE0) == 0xC0) {
		return 2;

	} else if ((uch(*i) & 0xF0) == 0xE0) {
		return 3;

	} else if ((uch(*i) & 0xF8) == 0xF0) {
		return 4;

	} else {
		OtError("Invalid codepoint in UTF-8 string");
	}

#if !_WIN32
	return 0;
#endif
}


//
//	OtCodePoint::skipBOM
//

std::string::const_iterator OtCodePoint::skipBOM(std::string::const_iterator i, std::string::const_iterator end) {
	// skip Byte Order Mark (BOM) just in case there is one

	// Note: the standard states that:
	// Use of a BOM is neither required nor recommended for UTF-8
	static constexpr char bom1 = static_cast<char>(0xEF);
	static constexpr char bom2 = static_cast<char>(0xBB);
	static constexpr char bom3 = static_cast<char>(0xBF);
	return ((end - i) >= 3 && i[0] == bom1 && i[1] == bom2 && i[2] == bom3) ? i + 3 : i;
}


//
//	OtCodePoint::read
//

std::string::const_iterator OtCodePoint::read(std::string::const_iterator i, std::string::const_iterator end, char32_t* codepoint) {
	// parse a UTF-8 sequence into a unicode codepoint
	if (i < end && (uch(*i) & 0x80) == 0) {
		*codepoint = uch(*i);
		i++;

	} else if (i + 1 < end && (uch(*i) & 0xE0) == 0xC0) {
		*codepoint = ((uch(*i) & 0x1f) << 6) | (uch(*(i + 1)) & 0x3f);
		i += 2;

	} else if (i + 2 < end && (uch(*i) & 0xF0) == 0xE0) {
		*codepoint = ((uch(*i) & 0x0f) << 12) | ((uch(*(i + 1)) & 0x3f) << 6) | (uch(*(i + 2)) & 0x3f);
		i += 3;

	} else if (i + 3 < end && (uch(*i) & 0xF8) == 0xF0) {
		*codepoint = ((uch(*i) & 0x07) << 18) | ((uch(*(i + 1)) & 0x3f) << 12) | ((uch(*(i + 2)) & 0x3f) << 6) | (uch(*(i + 3)) & 0x3f);
		i += 4;

	} else {
		*codepoint = invalidCodePoint;
		i++;
	}

	return i;
}


//
//	OtCodePoint::write
//

std::string::iterator OtCodePoint::write(std::string::iterator i, char32_t codepoint) {
	// generate UTF-8 sequence from a unicode codepoint
	if (codepoint < 0x80) {
		*i++ = sch(codepoint);

	} else if (codepoint < 0x800) {
		*i++ = sch(0xc0 | ((codepoint >> 6) & 0x1f));
		*i++ = sch(0x80 | (codepoint & 0x3f));

	} else if (codepoint < 0x10000) {
		*i++ = sch(0xe0 | ((codepoint >> 12) & 0x0f));
		*i++ = sch(0x80 | ((codepoint >> 6) & 0x3f));
		*i++ = sch(0x80 | (codepoint & 0x3f));

	} else if (codepoint >= 0x110000) {
		codepoint = invalidCodePoint;
		*i++ = sch(0xe0 | ((codepoint >> 12) & 0x0f));
		*i++ = sch(0x80 | ((codepoint >> 6) & 0x3f));
		*i++ = sch(0x80 | (codepoint & 0x3f));

	} else {
		*i++ = sch(0xf0 | ((codepoint >> 18) & 0x07));
		*i++ = sch(0x80 | ((codepoint >> 12) & 0x3f));
		*i++ = sch(0x80 | ((codepoint >> 6) & 0x3f));
		*i++ = sch(0x80 | (codepoint & 0x3f));
	}

	return i;
}


//
//	OtCodePoint::isAlphabetic
//

bool OtCodePoint::isAlphabetic(char32_t codepoint) {
	if (codepoint < 0x7f) {
		return static_cast<unsigned>((codepoint | 32) - 'a') < 26;

	} else if (codepoint < 0x10000) {
		return rangeContains(letters16, static_cast<char16_t>(codepoint));

	} else {
		return rangeContains(letters32, codepoint);
	}
}


//
//	OtCodePoint::isNumeric
//

bool OtCodePoint::isNumeric(char32_t codepoint) {
	if (codepoint < 0x7f) {
		return static_cast<unsigned>(codepoint - '0') < 10;

	} else if (codepoint < 0x10000) {
		return rangeContains(numbers16, static_cast<char16_t>(codepoint));

	} else {
		return rangeContains(numbers32, codepoint);
	}
}


//
//	OtCodePoint::isWhiteSpace
//

bool OtCodePoint::isWhiteSpace(char32_t codepoint) {
	if (codepoint < 0x7f) {
		return codepoint == ' ' || static_cast<unsigned>(codepoint - '\t') < 5;

	} else if (codepoint < 0x10000) {
		return rangeContains(whitespace16, static_cast<char16_t>(codepoint));

	} else {
		return false;
	}
}


//
//	OtCodePoint::isXidStart
//

bool OtCodePoint::isXidStart(char32_t codepoint) {
	if (codepoint < 0x7f) {
		return codepoint == '_' || static_cast<unsigned>((codepoint | 32) - 'a') < 26;

	} else if (codepoint < 0x10000) {
		return rangeContains(xidStart16, static_cast<char16_t>(codepoint));

	} else {
		return rangeContains(xidStart32, codepoint);
	}
}


//
//	OtCodePoint::isXidContinue
//

bool OtCodePoint::isXidContinue(char32_t codepoint) {
	if (codepoint < 0x7f) {
		return codepoint == '_' || (static_cast<unsigned>((codepoint | 32) - 'a') < 26) || (static_cast<unsigned>(codepoint - '0') < 10);

	} else if (codepoint < 0x10000) {
		return rangeContains(xidContinue16, static_cast<char16_t>(codepoint));

	} else {
		return rangeContains(xidContinue32, static_cast<char16_t>(codepoint));
	}
}


//
//	OtCodePoint::isLowerCase
//

bool OtCodePoint::isLowerCase(char32_t codepoint) {
	if (codepoint < 0x7f) {
		return static_cast<unsigned>(codepoint - 'a') < 26;

	} else if (codepoint < 0x10000) {
		return rangeContains(lower16, static_cast<char16_t>(codepoint));

	} else {
		return rangeContains(lower32, codepoint);
	}
}


//
//	OtCodePoint::isUpperCase
//

bool OtCodePoint::isUpperCase(char32_t codepoint) {
	if (codepoint < 0x7f) {
		return static_cast<unsigned>(codepoint - 'A') < 26;

	} else if (codepoint < 0x10000) {
		return rangeContains(upper16, static_cast<char16_t>(codepoint));

	} else {
		return rangeContains(upper32, codepoint);
	}
}


//
//	OtCodePoint::toLowerCase
//

char32_t OtCodePoint::toLowerCase(char32_t codepoint) {
	if (codepoint < 0x7f) {
		return (static_cast<unsigned>(codepoint - 'A') < 26) ? codepoint | 32 : codepoint;

	} else if (codepoint < 0x10000) {
		return caseRangeToLower(case16, static_cast<char16_t>(codepoint));

	} else {
		return caseRangeToLower(case32, static_cast<char32_t>(codepoint));
	}
}


//
//	OtCodePoint::toUpperCase
//

char32_t OtCodePoint::toUpperCase(char32_t codepoint) {
	if (codepoint < 0x7f) {
		return (static_cast<unsigned>(codepoint - 'a') < 26) ? codepoint & 0x5f : codepoint;

	} else if (codepoint < 0x10000) {
		return caseRangeToUpper(case16, static_cast<char16_t>(codepoint));

	} else {
		return caseRangeToUpper(case32, static_cast<char32_t>(codepoint));
	}
}
