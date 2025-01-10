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
//	GetLanguageDefinition
//

static TextEditor::Language& GetLanguageDefinition() {
	static bool initialized = false;
	static TextEditor::Language language;

	if (!initialized) {
		language.name = "ObjectTalk";
		language.commentStart = "/*";
		language.commentEnd = "*/";
		language.singleLineComment = "//";
		language.singleLineCommentAlt = "#";
		language.hasSingleQuoteStrings = false;
		language.hasDoubleQuoteStrings = true;
		language.stringEscape = '\\';

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

		for (auto& keyword : keywords) { language.keywords.insert(keyword); }
		for (auto& declaration : declarations) { language.declarations.insert(declaration); }
		for (auto& identifier : functions) { language.identifiers.insert(identifier); }
		for (auto& identifier : constants) { language.identifiers.insert(identifier); }

		language.isPunctuation = TextEditor::Language::isCStylePunctuation;
		language.isWord = TextEditor::Language::isCStyleWordCharacter;
		language.getIdentifier = TextEditor::Language::getCStyleIdentifier;
		language.getNumber = TextEditor::Language::getCStyleNumber;

		initialized = true;
	}

	return language;
}


//
//	OtObjectTalkEditor::OtObjectTalkEditor
//

OtObjectTalkEditor::OtObjectTalkEditor() {
	editor.SetLanguage(GetLanguageDefinition());
	editor.SetShowMatchingBrackets(true);
	editor.SetCompletePairedGlyphs(true);
}


//
//	OtObjectTalkEditor::highlightError
//

void OtObjectTalkEditor::highlightError(size_t line, const std::string& error) {
	std::map<int, std::string> markers;
	markers[(int) line] = error;
	editor.SetErrorMarkers(markers);
	scrollToLine = (int) line;
	focusOnEditor = true;
}


//
//	OtObjectTalkEditor::clearError
//

void OtObjectTalkEditor::clearError() {
	std::map<int, std::string> markers;
	editor.SetErrorMarkers(markers);
}
