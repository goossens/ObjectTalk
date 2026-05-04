//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
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
	// configure language
	auto language = OtObjectTalkLanguage::getDefinition();
	editor.SetLanguage(language);
	diff.SetLanguage(language);

	// activate autocomplete
	trieAutoComplete.Connect(&editor);
}


//
//	OtObjectTalkEditor::highlightError
//

void OtObjectTalkEditor::highlightError(size_t line, const std::string& error) {
	// line number is zero-based in text editor
	editor.AddMarker(line - 1, 0, IM_COL32(128, 0, 32, 128), "", error);
	editor.SetCursor(TextEditor::DocPos(line - 1, 0));
	editor.ScrollToLine(line - 1, TextEditor::Scroll::alignMiddle);
	editor.SetFocus();
}


//
//	OtObjectTalkEditor::clearError
//

void OtObjectTalkEditor::clearError() {
	editor.ClearMarkers();
}
