//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

#include "imgui.h"

#include "OtUi.h"

#include "OtConsole.h"
#include "OtObjectTalkEditor.h"
#include "OtObjectTalkLanguage.h"
#include "OtWorkspace.h"


//
//	Color palette
//

const static TextEditor::Palette colorPalette = { {
	0x7f7f7fff,	// Default
	0xc586c0ff,	// Keyword
	0xb5cea8ff,	// Number
	0xce9178ff,	// String
	0xce9178ff, // Char literal
	0xffffffff, // Punctuation
	0x808040ff,	// Preprocessor
	0x9cdcfeff, // Identifier
	0x4fc1ffff, // Known identifier
	0xa040c0ff, // Preproc identifier
	0x6a9955ff, // Comment (single line)
	0x6a9955ff, // Comment (multi line)
	0x101010ff, // Background
	0xe0e0e0ff, // Cursor
	0x2060a0ff, // Selection
	0x800020ff, // ErrorMarker
	0x404040ff, // ControlCharacter
	0x0080f040, // Breakpoint
	0x007070ff, // Line number
	0x00000040, // Current line fill
	0x80808040, // Current line fill (inactive)
	0xa0a0a040, // Current line edge
	0xaf00afff  // BracketHighlighting
} };


//
//	OtObjectTalkEditor::OtObjectTalkEditor
//

OtObjectTalkEditor::OtObjectTalkEditor() {
	editor.SetLanguageDefinition(OtObjectTalkLanguageGetDefinition());
	editor.SetPalette(colorPalette);
	editor.SetShowWhitespaces(true);
	editor.SetLineSpacing(1.25f);
	editor.SetShowShortTabGlyphs(true);
	editor.SetCompletePairedGlyphs(true);
	editor.SetHighlightPairedGlyphs(true);
}


//
//	OtObjectTalkEditor::load
//

void OtObjectTalkEditor::load() {
	// load text from file
	std::stringstream buffer;

	try {
		std::ifstream stream(path.string().c_str());

		if (stream.fail()) {
			OtError("Can't read from file [%s]", path.string().c_str());
		}

		buffer << stream.rdbuf();
		stream.close();

	} catch (std::exception& e) {
		OtError("Can't read from file [%s], error: %s", path.string().c_str(), e.what());
	}

	editor.SetText(buffer.str());
	version = editor.GetUndoIndex();
}


//
//	OtObjectTalkEditor::save
//

void OtObjectTalkEditor::save() {
	try {
		// write text to file
		std::ofstream stream(path.c_str());

		if (stream.fail()) {
			OtError("Can't write to file [%s]", path.c_str());
		}

		stream << editor.GetText();
		stream.close();

	} catch (std::exception& e) {
		OtError("Can't write to file [%s], error: %s", path.c_str(), e.what());
	}

	// reset current version number (marking the content as clean)
	version = editor.GetUndoIndex();
}


//
//	OtObjectTalkEditor::renderMenu
//

void OtObjectTalkEditor::renderMenu() {
#if __APPLE__
#define SHORTCUT "Cmd-"
#else
#define SHORTCUT "Ctrl-"
#endif

	// create menubar
	if (ImGui::BeginMenuBar()) {
		renderFileMenu();

		if (ImGui::BeginMenu("Edit")) {
			if (ImGui::MenuItem("Undo", SHORTCUT "Z", nullptr, editor.CanUndo())) { editor.Undo(); }
#if __APPLE__
			if (ImGui::MenuItem("Redo", "^" SHORTCUT "Z", nullptr, editor.CanRedo())) { editor.Redo(); }
#else
			if (ImGui::MenuItem("Redo", SHORTCUT "Y", nullptr, editor.CanRedo())) { editor.Redo(); }
#endif

			ImGui::Separator();
			if (ImGui::MenuItem("Cut", SHORTCUT "X", nullptr, editor.AnyCursorHasSelection())) { editor.Cut(); }
			if (ImGui::MenuItem("Copy", SHORTCUT "C", nullptr, editor.AnyCursorHasSelection())) { editor.Copy(); }
			if (ImGui::MenuItem("Paste", SHORTCUT "V", nullptr, ImGui::GetClipboardText() != nullptr)) { editor.Paste(); }

			ImGui::Separator();
			if (ImGui::MenuItem("Select All", SHORTCUT "A", nullptr, editor.GetText().size() != 0)) { editor.SelectAll(); }
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("View")) {
			renderCommonViewMenuItems();
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
	// create the editor
	ImGuiIO& io = ImGui::GetIO();
	ImGui::PushFont(io.Fonts->Fonts[uiEditorFont]);
	editor.Render("TextEditor");
	ImGui::PopFont();

	// scroll to line if required
	// (this has to be done here as the editor doesn't handle this well on open)
	if (scrollToLine) {
		editor.SetCursorPosition(TextEditor::Coordinates(scrollToLine - 1, 0));
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
//	TextEditor::ErrorMarkers markers;
//	markers[(int) line] = error;
//	editor.SetErrorMarkers(markers);
	scrollToLine = (int) line;
}


//
//	OtObjectTalkEditor::clearError
//

void OtObjectTalkEditor::clearError() {
//	TextEditor::ErrorMarkers markers;
//	editor.SetErrorMarkers(markers);
}


//
//	OtObjectTalkEditor::create
//

std::shared_ptr<OtObjectTalkEditor> OtObjectTalkEditor::create(const std::filesystem::path& path) {
	std::shared_ptr<OtObjectTalkEditor> editor = std::make_shared<OtObjectTalkEditor>();
	editor->setFilePath(path);

	if (editor->fileExists()) {
		editor->load();
	}

	return editor;
}
