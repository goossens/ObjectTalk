//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>

#include "imgui.h"

#include "OtMessageBus.h"
#include "OtPathTools.h"
#include "OtUi.h"

#include "OtConsole.h"
#include "OtObjectTalkEditor.h"
#include "OtObjectTalkLanguage.h"
#include "OtWorkspace.h"


//
//	Color palette
//

const static TextEditor::Palette colorPalette = { {
	0xd4d4d4ff,	// Default
	0xc586c0ff,	// Keyword
	0xb5cea8ff,	// Number
	0xce9178ff,	// String
	0xce9178ff, // Char literal
	0xffffffff, // Punctuation
	0x808040ff,	// Preprocessor
	0x9cdcfeff, // Identifier
	0x4fc1ffff, // Known identifier
	0x63c6abff, // Preproc identifier
	0x6a9955ff, // Comment (single line)
	0x6a9955ff, // Comment (multi line)
	0x1e1e1eff, // Background
	0xe0e0e0ff, // Cursor
	0x2060a0ff, // Selection
	0x800020ff, // ErrorMarker
	0x404040ff, // ControlCharacter
	0x0080f040, // Breakpoint
	0x6e7681ff, // Line number
	0xccccccff, // Current line number
} };


//
//	OtObjectTalkEditor::OtObjectTalkEditor
//

OtObjectTalkEditor::OtObjectTalkEditor() {
	editor.SetLanguageDefinition(OtObjectTalkLanguageGetDefinition());
	editor.SetPalette(colorPalette);
	editor.SetLineSpacing(1.25f);
	editor.SetShowWhitespacesEnabled(true);
	editor.SetShortTabsEnabled(true);
	editor.SetShowMatchingBrackets(true);
	editor.SetCompletePairedGlyphs(true);
}


//
//	OtObjectTalkEditor::newFile
//

void OtObjectTalkEditor::newFile(const std::string& path) {
	// setup the asset
	asset = path;
}


//
//	OtObjectTalkEditor::openFile
//

void OtObjectTalkEditor::openFile(const std::string& path) {
	// setup the asset
	asset.load(path, [this]() {
		editor.SetText(asset->getText());
		version = editor.GetUndoIndex();
	});
}


//
//	OtObjectTalkEditor::saveFile
//

void OtObjectTalkEditor::saveFile() {
	asset->setText(editor.GetText());
	asset.save();
	version = editor.GetUndoIndex();
}


//
//	OtObjectTalkEditor::saveAsFile
//

void OtObjectTalkEditor::saveAsFile(const std::string& path) {
	asset->setText(editor.GetText());
	asset.saveAs(path);
	version = editor.GetUndoIndex();
}


//
//	OtObjectTalkEditor::renderMenu
//

void OtObjectTalkEditor::renderMenu() {
	// create menubar
	if (ImGui::BeginMenuBar()) {
		renderFileMenu();

		if (ImGui::BeginMenu("Edit")) {
			if (ImGui::MenuItem("Undo", OT_UI_SHORTCUT "Z", nullptr, editor.CanUndo())) { editor.Undo(); }
#if __APPLE__
			if (ImGui::MenuItem("Redo", "^" OT_UI_SHORTCUT "Z", nullptr, editor.CanRedo())) { editor.Redo(); }
#else
			if (ImGui::MenuItem("Redo", OT_UI_SHORTCUT "Y", nullptr, editor.CanRedo())) { editor.Redo(); }
#endif

			ImGui::Separator();
			if (ImGui::MenuItem("Cut", OT_UI_SHORTCUT "X", nullptr, editor.AnyCursorHasSelection())) { editor.Cut(); }
			if (ImGui::MenuItem("Copy", OT_UI_SHORTCUT "C", nullptr, editor.AnyCursorHasSelection())) { editor.Copy(); }
			if (ImGui::MenuItem("Paste", OT_UI_SHORTCUT "V", nullptr, ImGui::GetClipboardText() != nullptr)) { editor.Paste(); }

			ImGui::Separator();
			if (ImGui::MenuItem("Select All", OT_UI_SHORTCUT "A", nullptr, editor.GetText().size() != 0)) { editor.SelectAll(); }
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("View")) {
			if (ImGui::MenuItem("Toggle Console")) { OtMessageBus::instance()->send("toggleconsole"); }
			ImGui::Separator();

			bool flag;
			flag = editor.IsShowWhitespacesEnabled(); if (ImGui::MenuItem("Show Whitespaces", nullptr, &flag)) { editor.SetShowWhitespacesEnabled(flag); };
			flag = editor.IsShowLineNumbersEnabled(); if (ImGui::MenuItem("Show Line Numbers", nullptr, &flag)) { editor.SetShowLineNumbersEnabled(flag); };
			flag = editor.IsShortTabsEnabled(); if (ImGui::MenuItem("Show Short Tabs", nullptr, &flag)) { editor.SetShortTabsEnabled(flag); };
			flag = editor.IsShowingMatchingBrackets(); if (ImGui::MenuItem("Show Matching Brackets", nullptr, &flag)) { editor.SetShowMatchingBrackets(flag); };
			flag = editor.IsCompletingPairedGlyphs(); if (ImGui::MenuItem("Complete Matchng Glyphs", nullptr, &flag)) { editor.SetCompletePairedGlyphs(flag); };

			ImGui::Separator();
			if (ImGui::MenuItem("Clear Errors")) { clearError(); }
			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}
}


//
//	OtObjectTalkEditor::renderEditor
//

void OtObjectTalkEditor::renderEditor() {
	// render the text editor
	ImGuiIO& io = ImGui::GetIO();
	ImGui::PushFont(io.Fonts->Fonts[uiEditorFont]);
	editor.Render("TextEditor");
	ImGui::PopFont();

	// scroll to line if required
	// (this has to be done here as the editor doesn't handle this well on open)
	if (scrollToLine) {
		editor.SetCursorPosition(scrollToLine - 1, 0);
		scrollToLine = 0;
	}
}


//
//	OtObjectTalkEditor::isDirty
//

bool OtObjectTalkEditor::isDirty() {
	return editor.GetUndoIndex() != version;
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
