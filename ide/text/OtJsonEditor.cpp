//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtJsonEditor.h"


//
//	OtJsonEditor::OtJsonEditor
//

OtJsonEditor::OtJsonEditor() {
	editor.SetLanguage(TextEditor::Language::Json());
	diff.SetLanguage(TextEditor::Language::Json());
}
