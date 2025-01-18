//	TextEditor - A syntax highlighting text editor for ImGui
//	Copyright (c) 2024-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "Tables.h"
#include "TextEditor.h"


//
//	Statistics
//

static constexpr size_t totalSizeOfTables16 =
	sizeof(lower16) +
	sizeof(upper16) +
	sizeof(numbers16) +
	sizeof(whitespace16) +
	sizeof(case16);

#if defined(IMGUI_USE_WCHAR32)
static constexpr size_t totalSizeOfTables32 =
	sizeof(lower32) +
	sizeof(upper32) +
	sizeof(numbers32) +
	sizeof(case32);

#else
static constexpr size_t totalSizeOfTables32 = 0;
#endif

static constexpr size_t totalSizeOfTables = totalSizeOfTables16 + totalSizeOfTables32;


//
//	TextEditor::CodePoint::read
//

std::string::const_iterator TextEditor::CodePoint::read(std::string::const_iterator i, ImWchar* codepoint) {
	// parse a UTF-8 sequence into a unicode codepoint
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
#if defined(IMGUI_USE_WCHAR32)
		*codepoint = ((*i & 0x07) << 18) | ((*(i + 1) & 0x3f) << 12) | ((*(i + 2) & 0x3f) << 6) | (*(i + 3) & 0x3f);
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

std::string::iterator TextEditor::CodePoint::write(std::string::iterator i, ImWchar codepoint) {
	// generate UTF-8 sequence from a unicode codepoint
	if (codepoint < 0x80) {
		*i++ = codepoint;

	} else if (codepoint < 0x800) {
		*i++ = 0xc0 | ((codepoint >> 6) & 0x1f);
		*i++ = 0x80 | (codepoint & 0x3f);

#if defined(IMGUI_USE_WCHAR32)
	} else if (codepoint < 0x10000) {
		*i++ = 0xe0 | ((codepoint >> 12) & 0x0f);
		*i++ = 0x80 | ((codepoint >> 6) & 0x3f);
		*i++ = 0x80 | (codepoint & 0x3f);

	} else if (codepoint >= 0x110000) {
		codepoint = IM_UNICODE_CODEPOINT_INVALID;
		*i++ = 0xe0 | ((codepoint >> 12) & 0x0f);
		*i++ = 0x80 | ((codepoint >> 6) & 0x3f);
		*i++ = 0x80 | (codepoint & 0x3f);

	} else {
		*i++ = 0xf0 | ((codepoint >> 18) & 0x07);
		*i++ = 0x80 | ((codepoint >> 12) & 0x3f);
		*i++ = 0x80 | ((codepoint >> 6) & 0x3f);
		*i++ = 0x80 | (codepoint & 0x3f);
	}

#else
	} else {
		*i++ = 0xe0 | ((codepoint >> 12) & 0x0f);
		*i++ = 0x80 | ((codepoint >> 6) & 0x3f);
		*i++ = 0x80 | (codepoint & 0x3f);
	}
#endif

	return i;
}


//
//	TextEditor::CodePoint::isWhiteSpace
//

bool TextEditor::CodePoint::isWhiteSpace(ImWchar codepoint) {
	if (codepoint < 0x7f) {
		return std::isspace(static_cast<unsigned char>(codepoint));

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
		return std::isalnum(static_cast<unsigned char>(codepoint));

#if defined(IMGUI_USE_WCHAR32)
	} else if (codepoint >= 0x10000) {
		return
			rangeContains(lower32, static_cast<ImWchar32>(codepoint)) ||
			rangeContains(upper32, static_cast<ImWchar32>(codepoint))||
			rangeContains(numbers32, static_cast<ImWchar32>(codepoint));
#endif

	} else {
		return
			rangeContains(lower16, static_cast<ImWchar16>(codepoint)) ||
			rangeContains(upper16, static_cast<ImWchar16>(codepoint)) ||
			rangeContains(numbers16, static_cast<ImWchar16>(codepoint));
	}
}


//
//	TextEditor::CodePoint::isLower
//

bool TextEditor::CodePoint::isLower(ImWchar codepoint) {
	if (codepoint < 0x7f) {
		return std::islower(static_cast<unsigned char>(codepoint));

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
		return std::isupper(static_cast<unsigned char>(codepoint));

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
		return std::toupper(static_cast<unsigned char>(codepoint));

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
		return std::tolower(static_cast<unsigned char>(codepoint));

#if defined(IMGUI_USE_WCHAR32)
	} else if (codepoint >= 0x10000) {
		return caseRangeToLower(case32, static_cast<char32_t>(codepoint));
#endif

	} else {
		return caseRangeToLower(case16, static_cast<char16_t>(codepoint));
	}
}
