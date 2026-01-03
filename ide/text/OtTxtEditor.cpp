//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtTxtEditor.h"


//
//	OtTxtEditor::OtTxtEditor
//

OtTxtEditor::OtTxtEditor() {
	editor.SetAutoIndentEnabled(false);
	editor.SetShowMatchingBrackets(false);
	editor.SetCompletePairedGlyphs(false);
}
