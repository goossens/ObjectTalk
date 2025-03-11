//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
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
	editor.SetLanguage(OtObjectTalkLanguage::getDefinition());
	diff.SetLanguage(OtObjectTalkLanguage::getDefinition());
}


//
//	OtObjectTalkEditor::highlightError
//

void OtObjectTalkEditor::highlightError(size_t line, const std::string& error) {
	// line number is zero-base in text editor
	int editorLine = static_cast<int>(line - 1);
	editor.AddMarker(editorLine, 0, IM_COL32(128, 0, 32, 128), "", error);
	editor.SetCursor(editorLine, 0);
	editor.ScrollToLine(editorLine, TextEditor::Scroll::alignMiddle);
}


//
//	OtObjectTalkEditor::clearError
//

void OtObjectTalkEditor::clearError() {
	editor.ClearMarkers();
}
