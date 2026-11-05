//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtMarkdownEditor.h"


//
//	OtMarkdownEditor::OtMarkdownEditor
//

OtMarkdownEditor::OtMarkdownEditor() {
	auto language = TextEditor::LookupLanguage("Markdown");
	editor.SetLanguage(language);
	editor.SetShowMatchingBrackets(false);
	diff.SetLanguage(language);
}
