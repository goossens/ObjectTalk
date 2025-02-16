//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtObjectTalkEditor.h"


//
//	getObjectTalkStyleNumber
//

static TextEditor::Iterator getObjectTalkStyleNumber(TextEditor::Iterator start, TextEditor::Iterator end) {
	// see if we have a string that looks like:
	// [-+]? ([1-9][0-9]* | "0"[Bb][01]+ | "0"[Oo][0-7]* | "0"[Xx][0-9A-Fa-f]* | "0"[0-7]*)
	auto i = start;
	if (i != end && (*i == '-' || *i == '+')) { i++; }
	if (i == end || !std::isdigit(*i)) { return start; }

	if (*i == '0') {
		if (++i == end) { return i;	}

		if (*i == 'b' || *i == 'B') {
			i++;
			if (i == end || !(*i == '0' || *i == '1')) { return start; }
			while (i != end && (*i == '0' || *i == '1')) { i++; }

		} else if (*i == 'o' || *i == 'O') {
			i++;
			if (i == end || !(*i >= '0' && *i <= '7')) { return start; }
			while (i != end && *i >= '0' && *i <= '7') { i++; }

		} else if (*i == 'x' || *i == 'X') {
			i++;
			if (i == end || !std::isxdigit(*i)) { return start; }
			while (i != end && std::isxdigit(*i)) { i++; }

			if (i != end && *i == '.') {
				i++;
				while (i != end && std::isdigit(*i)) { i++; }
			}

			if (i != end && (*i == 'e' || *i == 'E' || *i == 'p'|| *i == 'P')) {
				i++;
				if (i != end && (*i == '-' || *i == '+')) { i++; }
				if (i == end || !std::isdigit(*i)) { return start; }
				while (i != end && std::isdigit(*i)) { i++; }
			}

		} else {
			while (i != end && *i >= '0' && *i <= '7') { i++; }
		}

		while (i != end && (*i == 'u' || *i == 'U' || *i == 'l' || *i == 'L' || *i == 'z' || *i == 'Z')) {
			i++;
		}

	} else {
		while (i != end && std::isdigit(*i)) { i++; }

		if (i != end) {
			if (*i == '.') {
				i++;
				while (i != end && std::isdigit(*i)) { i++; }

				if (i != end && (*i == 'e'|| *i == 'E')) {
					i++;
					if (i != end && (*i == '-' || *i == '+')) { i++; }
					if (i == end || !std::isdigit(*i)) { return start; }
					while (i != end && std::isdigit(*i)) { i++; }
				}

				if (i != end && (*i == 'f'|| *i == 'F' || *i == 'l' || *i == 'L')) { i++; }

			} else if (*i == 'u' || *i == 'U' || *i == 'l' || *i == 'L') {
				i++;
			}
		}
	}

	return i;
}


//
//	getLanguageDefinition
//

static TextEditor::Language* getLanguageDefinition() {
	static bool initialized = false;
	static TextEditor::Language language;

	if (!initialized) {
		// inherit from the C language
		language = *TextEditor::Language::C();

		// set ObjectTalk unique features
		language.name = "ObjectTalk";
		language.preprocess = 0;
		language.singleLineCommentAlt = "#";
		language.hasSingleQuotedStrings = false;

		static const char* const keywords[] = {
			"catch", "do", "elif", "else", "for", "if", "in", "not", "return", "super", "throw", "try", "while"
		};

		static const char* const declarations[] = {
			"class", "function", "var"
		};

		static const char* const functions[] = {
			"assert", "import", "members", "print", "range"
		};

		static const char* const constants[] = {
			"e", "false", "null", "pi", "print", "true"
		};

		language.keywords.clear();
		language.declarations.clear();
		language.identifiers.clear();

		for (auto& keyword : keywords) { language.keywords.insert(keyword); }
		for (auto& declaration : declarations) { language.declarations.insert(declaration); }
		for (auto& identifier : functions) { language.identifiers.insert(identifier); }
		for (auto& identifier : constants) { language.identifiers.insert(identifier); }

		language.getNumber = getObjectTalkStyleNumber;
		initialized = true;
	}

	return &language;
}


//
//	OtObjectTalkEditor::OtObjectTalkEditor
//

OtObjectTalkEditor::OtObjectTalkEditor() {
	editor.SetLanguage(getLanguageDefinition());
}


//
//	OtObjectTalkEditor::highlightError
//

void OtObjectTalkEditor::highlightError(size_t line, const std::string& error) {
	editor.AddMarker(static_cast<int>(line - 1), 0, IM_COL32(128, 0, 32, 128), "", error);
	scrollToLine = static_cast<int>(line);
	focusOnEditor = true;
}


//
//	OtObjectTalkEditor::clearError
//

void OtObjectTalkEditor::clearError() {
	editor.ClearMarkers();
}
