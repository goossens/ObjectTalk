//	TextEditor - A syntax highlighting text editor for Dear ImGui.
//	Copyright (c) 2024-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "TextEditor.h"


//
//	TextEditor::AutoCompleter::setConfig
//

void TextEditor::AutoCompleter::setConfig(const AutoCompleteConfig* c) {
	if (c) {
		config = *c;
		configured = true;

	} else {
		configured = false;
	}

	active = false;
}


//
//	TextEditor::AutoCompleter::handleShortcut
//

void TextEditor::AutoCompleter::handleShortcut() {
	if (!active && configured && config.triggersOnShortcut) {
		activate = true;
	}
}
