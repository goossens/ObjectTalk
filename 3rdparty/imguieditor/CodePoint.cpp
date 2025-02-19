//	TextEditor - A syntax highlighting text editor for ImGui
//	Copyright (c) 2024-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


// 	Fast ASCII ctype functions borrowed from the musl library
//	https://git.musl-libc.org/cgit/musl/tree/include/ctype.h


//
//	Include files
//

#include "Tables.h"
#include "TextEditor.h"


//
//	Internal type conversions because "char" is signed
//

static inline ImWchar uch(char c) {
	return static_cast<ImWchar>(c);
}

static inline char sch(ImWchar i) {
	return static_cast<char>(i);
}


//
//	skipBOM
//

std::string_view::const_iterator TextEditor::CodePoint::skipBOM(std::string_view::const_iterator i, std::string_view::const_iterator end) {
	// skip Byte Order Mark (BOM) just in case there is one

	// Note: the standard states that:
	// Use of a BOM is neither required nor recommended for UTF-8
	static constexpr char bom1 = static_cast<char>(0xEF);
	static constexpr char bom2 = static_cast<char>(0xBB);
	static constexpr char bom3 = static_cast<char>(0xBF);
	return ((end - i) >= 3 && i[0] == bom1 && i[1] == bom2 && i[2] == bom3) ? i + 3 : i;
}


//
//	TextEditor::CodePoint::read
//

std::string_view::const_iterator TextEditor::CodePoint::read(std::string_view::const_iterator i, std::string_view::const_iterator end, ImWchar *codepoint) {
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
#if defined(IMGUI_USE_WCHAR32)
		*codepoint = ((uch(*i) & 0x07) << 18) | ((uch(*(i + 1)) & 0x3f) << 12) | ((uch(*(i + 2)) & 0x3f) << 6) | (uch(*(i + 3)) & 0x3f);
#else
		*codepoint = IM_UNICODE_CODEPOINT_INVALID;
#endif
		i += 4;

	} else {
		*codepoint = IM_UNICODE_CODEPOINT_INVALID;
		i++;
	}

	return i;
}


//
//	TextEditor::CodePoint::write
//

size_t TextEditor::CodePoint::write(char* start, ImWchar codepoint) {
	// generate UTF-8 sequence from a unicode codepoint
	auto i = start;

	if (codepoint < 0x80) {
		*i++ = sch(codepoint);

	} else if (codepoint < 0x800) {
		*i++ = sch(0xc0 | ((codepoint >> 6) & 0x1f));
		*i++ = sch(0x80 | (codepoint & 0x3f));

#if defined(IMGUI_USE_WCHAR32)
	} else if (codepoint < 0x10000) {
		*i++ = sch(0xe0 | ((codepoint >> 12) & 0x0f));
		*i++ = sch(0x80 | ((codepoint >> 6) & 0x3f));
		*i++ = sch(0x80 | (codepoint & 0x3f));

	} else if (codepoint >= 0x110000) {
		codepoint = IM_UNICODE_CODEPOINT_INVALID;
		*i++ = sch(0xe0 | ((codepoint >> 12) & 0x0f));
		*i++ = sch(0x80 | ((codepoint >> 6) & 0x3f));
		*i++ = sch(0x80 | (codepoint & 0x3f));

	} else {
		*i++ = sch(0xf0 | ((codepoint >> 18) & 0x07));
		*i++ = sch(0x80 | ((codepoint >> 12) & 0x3f));
		*i++ = sch(0x80 | ((codepoint >> 6) & 0x3f));
		*i++ = sch(0x80 | (codepoint & 0x3f));

#else
	} else {
		*i++ = sch(0xe0 | ((codepoint >> 12) & 0x0f));
		*i++ = sch(0x80 | ((codepoint >> 6) & 0x3f));
		*i++ = sch(0x80 | (codepoint & 0x3f));
#endif
	}

	return i - start;
}


//
//	TextEditor::CodePoint::isLetter
//

bool TextEditor::CodePoint::isLetter(ImWchar codepoint) {
	if (codepoint < 0x7f) {
		return static_cast<unsigned>((codepoint | 32) - 'a') < 26;

#if defined(IMGUI_USE_WCHAR32)
	} else if (codepoint >= 0x10000) {
		return rangeContains(letters32, static_cast<ImWchar32>(codepoint));
#endif

	} else {
		return rangeContains(letters16, static_cast<ImWchar16>(codepoint));
	}
}


//
//	TextEditor::CodePoint::isNumber
//

bool TextEditor::CodePoint::isNumber(ImWchar codepoint) {
	if (codepoint < 0x7f) {
		return static_cast<unsigned>(codepoint - '0') < 10;

#if defined(IMGUI_USE_WCHAR32)
	} else if (codepoint >= 0x10000) {
		return rangeContains(numbers32, static_cast<ImWchar32>(codepoint));
#endif

	} else {
		return rangeContains(numbers16, static_cast<ImWchar16>(codepoint));
	}
}


//
//	TextEditor::CodePoint::isXidStart
//

bool TextEditor::CodePoint::isXidStart(ImWchar codepoint) {
	if (codepoint < 0x7f) {
		return codepoint == '_' || static_cast<unsigned>((codepoint | 32) - 'a') < 26;

#if defined(IMGUI_USE_WCHAR32)
	} else if (codepoint >= 0x10000) {
		return rangeContains(xidStart32, static_cast<ImWchar32>(codepoint));
#endif

	} else {
		return rangeContains(xidStart16, static_cast<ImWchar16>(codepoint));
	}
}


//
//	TextEditor::CodePoint::isWhiteSpace
//

bool TextEditor::CodePoint::isWhiteSpace(ImWchar codepoint) {
	if (codepoint < 0x7f) {
		return codepoint == ' ' || static_cast<unsigned>(codepoint - '\t') < 5;

#if defined(IMGUI_USE_WCHAR32)
	} else if (codepoint >= 0x10000) {
		return false;
#endif

	} else {
		return rangeContains(whitespace16, static_cast<ImWchar16>(codepoint));
	}
}


//
//	TextEditor::CodePoint::isWord
//

bool TextEditor::CodePoint::isWord(ImWchar codepoint) {
	if (codepoint < 0x7f) {
		return (static_cast<unsigned>((codepoint | 32) - 'a') < 26) || (static_cast<unsigned>(codepoint - '0') < 10);

#if defined(IMGUI_USE_WCHAR32)
	} else if (codepoint >= 0x10000) {
		return
			rangeContains(letters32, static_cast<ImWchar32>(codepoint)) ||
			rangeContains(numbers32, static_cast<ImWchar32>(codepoint));
#endif

	} else {
		return
			rangeContains(letters16, static_cast<ImWchar16>(codepoint)) ||
			rangeContains(numbers16, static_cast<ImWchar16>(codepoint));
	}
}


//
//	TextEditor::CodePoint::isXidContinue
//

bool TextEditor::CodePoint::isXidContinue(ImWchar codepoint) {
	if (codepoint < 0x7f) {
		return codepoint == '_' || (static_cast<unsigned>((codepoint | 32) - 'a') < 26) || (static_cast<unsigned>(codepoint - '0') < 10);

#if defined(IMGUI_USE_WCHAR32)
	} else if (codepoint >= 0x10000) {
		return rangeContains(xidContinue32, static_cast<ImWchar16>(codepoint));
#endif

	} else {
		return rangeContains(xidContinue16, static_cast<ImWchar16>(codepoint));
	}
}


//
//	TextEditor::CodePoint::isLower
//

bool TextEditor::CodePoint::isLower(ImWchar codepoint) {
	if (codepoint < 0x7f) {
		return static_cast<unsigned>(codepoint - 'a') < 26;

#if defined(IMGUI_USE_WCHAR32)
	} else if (codepoint >= 0x10000) {
		return rangeContains(lower32, static_cast<ImWchar32>(codepoint));
#endif

	} else {
		return rangeContains(lower16, static_cast<ImWchar16>(codepoint));
	}
}


//
//	TextEditor::CodePoint::isUpper
//

bool TextEditor::CodePoint::isUpper(ImWchar codepoint) {
	if (codepoint < 0x7f) {
		return static_cast<unsigned>(codepoint - 'A') < 26;

#if defined(IMGUI_USE_WCHAR32)
	} else if (codepoint >= 0x10000) {
		return rangeContains(upper32, static_cast<ImWchar32>(codepoint));
#endif

	} else {
		return rangeContains(upper16, static_cast<ImWchar16>(codepoint));
	}
}


//
//	TextEditor::CodePoint::toUpper
//

ImWchar TextEditor::CodePoint::toUpper(ImWchar codepoint) {
	if (codepoint < 0x7f) {
		return (static_cast<unsigned>(codepoint - 'a') < 26) ? codepoint & 0x5f : codepoint;

#if defined(IMGUI_USE_WCHAR32)
	} else if (codepoint >= 0x10000) {
		return caseRangeToUpper(case32, static_cast<char32_t>(codepoint));
#endif

	} else {
		return caseRangeToUpper(case16, static_cast<char16_t>(codepoint));
	}
}


//
//	TextEditor::CodePoint::toLower
//

ImWchar TextEditor::CodePoint::toLower(ImWchar codepoint) {
	if (codepoint < 0x7f) {
		return (static_cast<unsigned>(codepoint - 'A') < 26) ? codepoint | 32 : codepoint;

#if defined(IMGUI_USE_WCHAR32)
	} else if (codepoint >= 0x10000) {
		return caseRangeToLower(case32, static_cast<char32_t>(codepoint));
#endif

	} else {
		return caseRangeToLower(case16, static_cast<char16_t>(codepoint));
	}
}
