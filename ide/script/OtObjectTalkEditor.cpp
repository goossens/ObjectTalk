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
#include "OtScriptRunner.h"
#include "OtWorkspace.h"


//
//	colorPalette (** means changed from default dark palette)
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
//	OtObjectTalkEditorClass::OtObjectTalkEditorClass
//

OtObjectTalkEditorClass::OtObjectTalkEditorClass() {
	editor.SetLanguageDefinition(OtObjectTalkLanguageGetDefinition());
	editor.SetPalette(colorPalette);
	editor.SetShowWhitespaces(true);
	editor.SetShowShortTabGlyphs(true);
	editor.SetImGuiChildIgnored(true);
}


//
//	OtObjectTalkEditorClass::load
//

void OtObjectTalkEditorClass::load() {
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
//	OtObjectTalkEditorClass::save
//

void OtObjectTalkEditorClass::save() {
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
//	OtObjectTalkEditorClass::render
//

void OtObjectTalkEditorClass::render() {
	// determine editor height
	auto displaySize = ImGui::GetIO().DisplaySize.y;
	auto minEditorWidth = displaySize * 0.05f;
	auto maxEditorWidth = displaySize * 0.90f;

	if (editorHeight < 0.0) {
		editorHeight =  displaySize * 0.8;

	} else {
		editorHeight = std::clamp(editorHeight, minEditorWidth, maxEditorWidth);
	}

	// create the window
	ImGui::BeginChild(
		"scene",
		ImVec2(0.0, 0.0),
		true,
		ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_MenuBar);

	// render the menu
	renderMenu();

	// render splitter
	OtUiSplitterVertical(&editorHeight, minEditorWidth, maxEditorWidth);

	// render the editor
	renderEditor();

	// render the console
	OtConsoleClass::instance()->render();
	ImGui::EndChild();
}


//
//	OtObjectTalkEditorClass::renderMenu
//

void OtObjectTalkEditorClass::renderMenu() {
	// determine status
	bool runnable = !OtScriptRunnerClass::instance()->isRunning() && !isDirty() && fileExists();

	// get keyboard state to handle keyboard shortcuts
	ImGuiIO& io = ImGui::GetIO();
	auto isOSX = io.ConfigMacOSXBehaviors;
	auto alt = io.KeyAlt;
	auto ctrl = io.KeyCtrl;
	auto shift = io.KeyShift;
	auto super = io.KeySuper;
	auto isShortcut = (isOSX ? (super && !ctrl) : (ctrl && !super)) && !alt && !shift;

	// handle keyboard shortcuts
	if (isShortcut) {
		if (ImGui::IsKeyPressed(ImGuiKey_B)) {
			compile();

		} else if (ImGui::IsKeyPressed(ImGuiKey_R) && runnable) {
			run();
		}
	}

#if __APPLE__
#define SHORTCUT "Cmd-"
#else
#define SHORTCUT "Ctrl-"
#endif

	// create menubar
	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("File")) {  // shortcuts handled by workspace
			if (ImGui::MenuItem("New", SHORTCUT "N")) { OtWorkspaceClass::instance()->newFile(); }
			if (ImGui::MenuItem("Open...", SHORTCUT "O")) { OtWorkspaceClass::instance()->openFile(); }
			ImGui::Separator();

			if (fileExists()) {
				if (ImGui::MenuItem("Save", SHORTCUT "S", nullptr, isDirty())) { OtWorkspaceClass::instance()->saveFile(); }
				if (ImGui::MenuItem("Save As...")) { OtWorkspaceClass::instance()->saveAsFile(); }

			} else {
				if (ImGui::MenuItem("Save As...", SHORTCUT "S", nullptr, isDirty())) { OtWorkspaceClass::instance()->saveAsFile(); }
			}

			ImGui::Separator();
			if (ImGui::MenuItem("Close", SHORTCUT "W")) { OtWorkspaceClass::instance()->closeFile(); }

			ImGui::EndMenu();
		}

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

		if (ImGui::BeginMenu("Program")) {
			if (ImGui::MenuItem("Build", SHORTCUT "B")) { compile(); }
			if (ImGui::MenuItem("Run", SHORTCUT "R", nullptr, runnable)) { run(); }
			ImGui::Separator();
			if (ImGui::MenuItem("Clear Error")) { clearError(); }
			if (ImGui::MenuItem("Clear Console")) { OtConsoleClass::instance()->clear(); }

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}
}


//
//	OtObjectTalkEditorClass::renderEditor
//

void OtObjectTalkEditorClass::renderEditor() {
	// create the window
	ImGui::BeginChild("editor", ImVec2(0.0, editorHeight), true);

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
//	OtObjectTalkEditorClass::compile
//

void OtObjectTalkEditorClass::compile() {
}


//
//	OtObjectTalkEditorClass::run
//

void OtObjectTalkEditorClass::run() {
	// reset errors
	clearError();

	// let the script runner do its job
	OtScriptRunnerClass::instance()->run(path);
}


//
//	OtObjectTalkEditorClass::isDirty
//

bool OtObjectTalkEditorClass::isDirty() {
	return editor.GetUndoCount() != version;
}


//
//	OtObjectTalkEditorClass::highlightError
//

void OtObjectTalkEditorClass::highlightError(size_t line, const std::string& error) {
	TextEditor::ErrorMarkers markers;
	markers[(int) line] = error;
	editor.SetErrorMarkers(markers);
	scrollToLine = (int) line;
}


//
//	OtObjectTalkEditorClass::clearError
//

void OtObjectTalkEditorClass::clearError() {
	TextEditor::ErrorMarkers markers;
	editor.SetErrorMarkers(markers);
}


//
//	OtObjectTalkEditorClass::create
//

OtObjectTalkEditor OtObjectTalkEditorClass::create(const std::filesystem::path& path) {
	OtObjectTalkEditor editor = std::make_shared<OtObjectTalkEditorClass>();
	editor->setFilePath(path);

	if (editor->fileExists()) {
		editor->load();
	}

	return editor;
}
