//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtObjectTalkLanguage.h"


//
//	ObjectTalk language definition
//

static bool TokenizeCStyleString(const char* in_begin, const char* in_end, const char*& out_begin, const char*& out_end) {
	const char* p = in_begin;

	if (*p == '"') {
		p++;

		while (p < in_end) {
			// handle end of string
			if (*p == '"') {
				out_begin = in_begin;
				out_end = p + 1;
				return true;
			}

			// handle escape character for "
			if (*p == '\\' && p + 1 < in_end && p[1] == '"') {
				p++;
			}

			p++;
		}
	}

	return false;
}

static bool TokenizeCStyleIdentifier(const char* in_begin, const char* in_end, const char*& out_begin, const char*& out_end) {
	const char * p = in_begin;

	if ((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || *p == '_') {
		p++;

		while ((p < in_end) && ((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || (*p >= '0' && *p <= '9') || *p == '_')) {
			p++;
		}

		out_begin = in_begin;
		out_end = p;
		return true;
	}

	return false;
}

static bool TokenizeCStyleNumber(const char* in_begin, const char* in_end, const char*& out_begin, const char*& out_end) {
	const char * p = in_begin;
	const bool startsWithNumber = *p >= '0' && *p <= '9';

	if (*p != '+' && *p != '-' && !startsWithNumber) {
		return false;
	}

	p++;
	bool hasNumber = startsWithNumber;

	while (p < in_end && (*p >= '0' && *p <= '9')) {
		hasNumber = true;
		p++;
	}

	if (hasNumber == false) {
		return false;
	}

	bool isFloat = false;
	bool isHex = false;
	bool isBinary = false;

	if (p < in_end) {
		if (*p == '.') {
			isFloat = true;
			p++;

			while (p < in_end && (*p >= '0' && *p <= '9')) {
				p++;
			}

		} else if (*p == 'x' || *p == 'X') {
			// hex formatted integer of the type 0xef80
			isHex = true;
			p++;

			while (p < in_end && ((*p >= '0' && *p <= '9') || (*p >= 'a' && *p <= 'f') || (*p >= 'A' && *p <= 'F'))) {
				p++;
			}

		} else if (*p == 'b' || *p == 'B') {
			// binary formatted integer of the type 0b01011101
			isBinary = true;
			p++;

			while (p < in_end && (*p >= '0' && *p <= '1')) {
				p++;
			}
		}
	}

	if (isHex == false && isBinary == false) {
		// floating point exponent
		if (p < in_end && (*p == 'e' || *p == 'E')) {
			isFloat = true;
			p++;

			if (p < in_end && (*p == '+' || *p == '-')) {
				p++;
			}

			bool hasDigits = false;

			while (p < in_end && (*p >= '0' && *p <= '9')) {
				hasDigits = true;
				p++;
			}

			if (hasDigits == false) {
				return false;
			}
		}

		// single precision floating point type
		if (p < in_end && *p == 'f') {
			p++;
		}
	}

	if (isFloat == false) {
		// integer size type
		while (p < in_end && (*p == 'u' || *p == 'U' || *p == 'l' || *p == 'L')) {
			p++;
		}
	}

	out_begin = in_begin;
	out_end = p;
	return true;
}

static bool TokenizeCStylePunctuation(const char* in_begin, const char* in_end, const char*& out_begin, const char*& out_end) {
	switch (*in_begin) {
		case '[':
		case ']':
		case '{':
		case '}':
		case '!':
		case '%':
		case '^':
		case '&':
		case '*':
		case '(':
		case ')':
		case '-':
		case '+':
		case '=':
		case '~':
		case '|':
		case '<':
		case '>':
		case '?':
		case ':':
		case '/':
		case ';':
		case ',':
		case '.':
			out_begin = in_begin;
			out_end = in_begin + 1;
			return true;
	}

	return false;
}

const TextEditor::LanguageDefinition& OtObjectTalkLanguageGetDefinition() {
	static bool initialized = false;
	static TextEditor::LanguageDefinition langDef;

	if (!initialized) {
		static const char* const otDeclarations[] = {
			"function", "var"
		};

		for (auto& d : otDeclarations) {
			langDef.mIdentifiers.insert(d);
		}

		static const char* const otKeywords[] = {
			"catch", "class", "do", "elif", "else", "for", "if", "in", "not", "return", "super", "throw", "try", "while"
		};

		for (auto& k : otKeywords) {
			langDef.mKeywords.insert(k);
		}

		static const char* const otFunctions[] = {
			"assert", "import", "members", "print", "range"
		};

		for (auto& f : otFunctions) {
			langDef.mIdentifiers.insert(f);
		}

		static const char* const otConstants[] = {
			"e", "false", "null", "pi", "print", "true"
		};

		for (auto& c : otConstants) {
			langDef.mIdentifiers.insert(c);
		}

		langDef.mTokenize = [](const char* in_begin, const char* in_end, const char*& out_begin, const char*& out_end, TextEditor::PaletteIndex& paletteIndex) -> bool {
			paletteIndex = TextEditor::PaletteIndex::Max;

			while (in_begin < in_end && isascii(*in_begin) && isblank(*in_begin)) {
				in_begin++;
			}

			if (in_begin == in_end) {
				out_begin = in_end;
				out_end = in_end;
				paletteIndex = TextEditor::PaletteIndex::Default;

			} else if (TokenizeCStyleString(in_begin, in_end, out_begin, out_end)) {
				paletteIndex = TextEditor::PaletteIndex::String;

			} else if (TokenizeCStyleIdentifier(in_begin, in_end, out_begin, out_end)) {
				paletteIndex = TextEditor::PaletteIndex::Identifier;

			} else if (TokenizeCStyleNumber(in_begin, in_end, out_begin, out_end)) {
				paletteIndex = TextEditor::PaletteIndex::Number;

			} else if (TokenizeCStylePunctuation(in_begin, in_end, out_begin, out_end)) {
				paletteIndex = TextEditor::PaletteIndex::Punctuation;
			}

			return paletteIndex != TextEditor::PaletteIndex::Max;
		};

		langDef.mCommentStart = "/*";
		langDef.mCommentEnd = "*/";
		langDef.mSingleLineComment = "//";
		langDef.mCaseSensitive = true;
		langDef.mName = "ObjectTalk";
		initialized = true;
	}

	return langDef;
}
