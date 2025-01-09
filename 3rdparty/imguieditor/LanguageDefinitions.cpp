//	TextEditor - A syntax highlighting text editor for ImGui
//	Copyright (c) 2024-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "TextEditor.h"


static bool identifierStart[128] = {
	false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	false,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,
	 true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true, false, false, false, false,  true,
	false,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,
	 true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true, false, false, false, false, false
};

static bool identifierNoneStart[128] = {
	false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	 true,  true,  true,  true,  true,  true,  true,  true,  true,  true, false, false, false, false, false, false,
	false,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,
	 true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true, false, false, false, false,  true,
	false,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,
	 true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true, false, false, false, false, false
};

static bool punctuation[128] = {
	false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	false,  true, false, false, false,  true,  true, false,  true,  true,  true,  true,  true,  true,  true,  true,
	false, false, false, false, false, false, false, false, false, false,  true,  true,  true,  true,  true,  true,
	false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	false, false, false, false, false, false, false, false, false, false, false,  true, false,  true,  true, false,
	false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	false, false, false, false, false, false, false, false, false, false, false,  true,  true,  true,  true, false,
};


//
//	TextEditor::Language::isCStylePunctuation
//

bool TextEditor::Language::isCStylePunctuation(ImWchar character) {
	return character < 127 ? punctuation[character] : false;
}


//
//	TextEditor::Language::isCStyleWordCharacter
//

bool TextEditor::Language::isCStyleWordCharacter(ImWchar character) {
	return character < 127 ? identifierNoneStart[character] : false;
}


//
//	TextEditor::Language::getCStyleIdentifier
//

TextEditor::Iterator TextEditor::Language::getCStyleIdentifier(Iterator start, Iterator end) {
	if (*start < 128 && identifierStart[*start]) {
		start++;

		while (start < end && *start < 128 && identifierNoneStart[*start]) {
			start++;
		}
	}

	return start;
}

//
//	TextEditor::Language::getCStyleNumber
//

TextEditor::Iterator TextEditor::Language::getCStyleNumber(Iterator start, Iterator end) {
	auto i = start;
	auto startsWithNumber = *i >= '0' && *i <= '9';

	if (*i != '+' && *i != '-' && !startsWithNumber) {
		return start;
	}

	i++;
	auto hasNumber = startsWithNumber;

	while (i < end && (*i >= '0' && *i <= '9')) {
		hasNumber = true;
		i++;
	}

	if (hasNumber == false) {
		return start;
	}

	auto isFloat = false;
	auto isNonFloat = false;

	if (i < end) {
		if (*i == '.') {
			isFloat = true;
			i++;

			while (i < end && (*i >= '0' && *i <= '9')) {
				i++;
			}

		} else if (*i == 'x' || *i == 'X') {
			// hex formatted integer of the type 0xef80
			isNonFloat = true;
			i++;

			while (i < end && ((*i >= '0' && *i <= '9') || (*i >= 'a' && *i <= 'f') || (*i >= 'A' && *i <= 'F'))) {
				i++;
			}

		} else if (*i == '0') {
			// octal formatted integer of the type 0784
			isNonFloat = true;
			i++;

			while (i < end && (*i >= '0' && *i <= '7')) {
				i++;
			}

		} else if (*i == 'b' || *i == 'B') {
			// binary formatted integer of the type 0b01011101
			isNonFloat = true;
			i++;

			while (i < end && (*i >= '0' && *i <= '1')) {
				i++;
			}
		}
	}

	if (!isNonFloat) {
		// floating point exponent
		if (i < end && (*i == 'e' || *i == 'E')) {
			isFloat = true;
			i++;

			if (i < end && (*i == '+' || *i == '-')) {
				i++;
			}

			bool hasDigits = false;

			while (i < end && (*i >= '0' && *i <= '9')) {
				hasDigits = true;
				i++;
			}

			if (!hasDigits) {
				return start;
			}
		}

		// single precision floating point type
		if (i < end && *i == 'f') {
			i++;
		}
	}

	if (isFloat == false) {
		// integer size type
		while (i < end && (*i == 'u' || *i == 'U' || *i == 'l' || *i == 'L')) {
			i++;
		}
	}

	return i;
}
