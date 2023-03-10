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
//	colorPalette (** means 'changed from default dark palette')
//

const static TextEditor::Palette colorPalette = { {
	0xff7f7f7f,	// Default
	0xffc086c5,	// Keyword **
	0xffa8ceb5,	// Number **
	0xff7891ce,	// String **
	0xff7891ce, // Char literal **
	0xffffffff, // Punctuation
	0xff408080,	// Preprocessor
	0xfffedc9c, // Identifier **
	0xffffc14f, // Known identifier **
	0xffc040a0, // Preproc identifier
	0xff55996a, // Comment (single line) **
	0xff55996a, // Comment (multi line) **
	0xff101010, // Background
	0xffe0e0e0, // Cursor
	0x80a06020, // Selection
	0x800020ff, // ErrorMarker
	0xffaf00af, // BracketHighlighting
	0x40f08000, // Breakpoint
	0xff707000, // Line number
	0x40000000, // Current line fill
	0x40808080, // Current line fill (inactive)
	0x40a0a0a0, // Current line edge
	0xff505050, // White Space
	0xff404040, // White Space Tab
} };


//
//	OtObjectTalkEditor::OtObjectTalkEditor
//

OtObjectTalkEditor::OtObjectTalkEditor() {
	editor.SetLanguageDefinition(OtObjectTalkLanguageGetDefinition());
	editor.SetPalette(colorPalette);
	editor.SetShowWhitespaces(true);
	editor.SetShowShortTabGlyphs(true);
	editor.SetImGuiChildIgnored(true);
}


//
//	OtObjectTalkEditor::load
//

void OtObjectTalkEditor::load() {
	// load text from file
	std::stringstream buffer;

	try {
		std::ifstream stream(path.c_str());

		if (stream.fail()) {
			OtExcept("Can't read from file [%s]", path.c_str());
		}

		buffer << stream.rdbuf();
		stream.close();

	} catch (std::exception& e) {
		OtExcept("Can't read from file [%s], error: %s", path.c_str(), e.what());
	}

	editor.SetText(buffer.str());
	version = editor.GetUndoCount();
}


//
//	OtObjectTalkEditor::save
//

void OtObjectTalkEditor::save() {
	try {
		// write text to file
		std::ofstream stream(path.c_str());

		if (stream.fail()) {
			OtExcept("Can't write to file [%s]", path.c_str());
		}

		stream << editor.GetText();
		stream.close();

	} catch (std::exception& e) {
		OtExcept("Can't write to file [%s], error: %s", path.c_str(), e.what());
	}

	// reset current version number (marking the content as clean)
	version = editor.GetUndoCount();
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
			if (ImGui::MenuItem("Cut", SHORTCUT "X", nullptr, editor.HasSelection())) { editor.Cut(); }
			if (ImGui::MenuItem("Copy", SHORTCUT "C", nullptr, editor.HasSelection())) { editor.Copy(); }
			if (ImGui::MenuItem("Paste", SHORTCUT "V", nullptr, ImGui::GetClipboardText() != nullptr)) { editor.Paste(); }

			ImGui::Separator();
			if (ImGui::MenuItem("Select All", SHORTCUT "A", nullptr, editor.GetText().size() != 0)) { editor.SelectAll(); }
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("View")) {
			renderCommonViewMenuItems();
			if (ImGui::MenuItem("Clear errors")) { clearError(); }
			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}
}


//
//	OtObjectTalkEditor::renderEditor
//

void OtObjectTalkEditor::renderEditor() {
	// create the window
	ImGui::BeginChild("editor", ImVec2(0.0f, 0.0f), true);

	// create the editor
	editor.Render("TextEditor");

	// scroll to line if required
	// (this has to be done here as the editor doesn't handle this well on open)
	if (scrollToLine) {
		editor.SetCursorPosition(TextEditor::Coordinates(scrollToLine - 1, 0));
		scrollToLine = 0;
	}

	ImGui::EndChild();
}


//
//	OtObjectTalkEditor::isDirty
//

bool OtObjectTalkEditor::isDirty() {
	return editor.GetUndoCount() != version;
}


//
//	OtObjectTalkEditor::highlightError
//

void OtObjectTalkEditor::highlightError(size_t line, const std::string& error) {
	TextEditor::ErrorMarkers markers;
	markers[(int) line] = error;
	editor.SetErrorMarkers(markers);
	scrollToLine = (int) line;
}


//
//	OtObjectTalkEditor::clearError
//

void OtObjectTalkEditor::clearError() {
	TextEditor::ErrorMarkers markers;
	editor.SetErrorMarkers(markers);
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
