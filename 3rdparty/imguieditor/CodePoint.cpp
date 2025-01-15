//	TextEditor - A syntax highlighting text editor for ImGui
//	Copyright (c) 2024-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "TextEditor.h"


//
//	TextEditor::CodePoint::read
//

std::string::const_iterator TextEditor::CodePoint::read(std::string::const_iterator i, ImWchar* codepoint) const {
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

std::string::iterator TextEditor::CodePoint::write(std::string::iterator i, ImWchar codepoint) const {
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
//	TextEditor::CodePoint::isSpace
//

bool TextEditor::CodePoint::isSpace(ImWchar codepoint) const {
	// see if codepoint represents a whitespace
	return unicode.isSpace ?  unicode.isSpace(codepoint) : codepoint < 256 ? std::isspace(static_cast<unsigned char>(codepoint)) : false;
}


//
//	TextEditor::CodePoint::isWord
//

bool TextEditor::CodePoint::isWord(ImWchar codepoint) const {
	static bool word[128] = {
		false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
		false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
		false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
		 true,  true,  true,  true,  true,  true,  true,  true,  true,  true, false, false, false, false, false, false,
		false,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,
		 true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true, false, false, false, false,  true,
		false,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,
		 true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true, false, false, false, false, false
	};

	return unicode.isWord ? unicode.isWord(codepoint) : codepoint < 127 ? word[codepoint] : false;
}


//
//	TextEditor::CodePoint::isUpper
//

bool TextEditor::CodePoint::isUpper(ImWchar codepoint) const {
	return unicode.isUpper ? unicode.isUpper(codepoint) : codepoint < 256 ? std::isupper(static_cast<unsigned char>(codepoint)) : codepoint;
}


//
//	TextEditor::CodePoint::isLower
//

bool TextEditor::CodePoint::isLower(ImWchar codepoint) const {
	return unicode.isLower ? unicode.isLower(codepoint) : codepoint < 256 ? std::islower(static_cast<unsigned char>(codepoint)) : codepoint;
}


//
//	TextEditor::CodePoint::toUpper
//

ImWchar TextEditor::CodePoint::toUpper(ImWchar codepoint) const {
	return unicode.toUpper ? unicode.toUpper(codepoint) : codepoint < 256 ? std::toupper(static_cast<unsigned char>(codepoint)) : codepoint;
}


//
//	TextEditor::CodePoint::toLower
//

ImWchar TextEditor::CodePoint::toLower(ImWchar codepoint) const {
	// convert a codepoint to lowercase
	return unicode.toLower ? unicode.toLower(codepoint) : codepoint < 256 ? std::tolower(static_cast<unsigned char>(codepoint)) : codepoint;
}
