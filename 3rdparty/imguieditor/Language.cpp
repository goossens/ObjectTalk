//	TextEditor - A syntax highlighting text editor for ImGui
//	Copyright (c) 2024-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cctype>

#include "TextEditor.h"

#include "languages/C.h"
#include "languages/Cpp.h"
#include "languages/Cs.h"
#include "languages/AngelScript.h"
#include "languages/Lua.h"
#include "languages/Python.h"
#include "languages/Glsl.h"
#include "languages/Hlsl.h"
#include "languages/Json.h"
#include "languages/Markdown.h"


//
//	TextEditor::Iterator::operator*
//

TextEditor::Iterator::reference TextEditor::Iterator::operator*() const {
	return static_cast<Line*>(line)->at(index).codepoint;
}


//
//	TextEditor::Iterator::operator->
//

TextEditor::Iterator::pointer TextEditor::Iterator::operator->() const {
	return &(static_cast<Line*>(line)->at(index).codepoint);
}
