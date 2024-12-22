//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtObjectTalkEditor.h"
#include "OtObjectTalkLanguage.h"


//
//	OtObjectTalkEditor::OtObjectTalkEditor
//

OtObjectTalkEditor::OtObjectTalkEditor() {
	editor.SetLanguageDefinition(OtObjectTalkLanguageGetDefinition());
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
}


//
//	OtObjectTalkEditor::clearError
//

void OtObjectTalkEditor::clearError() {
	std::map<int, std::string> markers;
	editor.SetErrorMarkers(markers);
}
