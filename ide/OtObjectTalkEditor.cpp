//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <fstream>
#include <iostream>
#include <sstream>

#include "imgui.h"
#include "ImGuiFileDialog.h"

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
	filename = "untitled";
}


//
//	OtObjectTalkEditorClass::render
//

void OtObjectTalkEditorClass::render(float width, float height) {
	// determine status
	bool dirty = editor.GetUndoCount() != version;
	bool runnable = !OtScriptRunnerClass::instance()->isRunning() && !dirty && filename != "untitled";

	// create the window
	ImGui::BeginChild(
		id.c_str(),
		ImVec2(width, height),
		true,
		ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_MenuBar);

	ImGui::PushID(this);

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
		if (dirty && ImGui::IsKeyPressed(ImGuiKey_S)) {
			if (filename == "untitled") {
				saveAsFile();

			} else {
				saveFile();
			}

		} else if (ImGui::IsKeyPressed(ImGuiKey_W)) {
			if (dirty) {
				confirmClose = true;

			} else {
				closeFile();
			}

		} else if (ImGui::IsKeyPressed(ImGuiKey_B)) {
			compileFile();

		} else if (ImGui::IsKeyPressed(ImGuiKey_R)) {
			if (runnable) {
				runFile();
			}
		}
	}

#if __APPLE__
#define SHORTCUT "Cmd-"
#else
#define SHORTCUT "Ctrl-"
#endif

	// create menubar
	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("New", SHORTCUT "N")) { newFile(); } // shortcut handled by workspace
			if (ImGui::MenuItem("Open...", SHORTCUT "O")) { openFile(); } // shortcut handled by workspace

			ImGui::Separator();

			if (filename == "untitled") {
				if (ImGui::MenuItem("Save As...", SHORTCUT "S", nullptr, dirty)) { saveAsFile(); }

			} else {
				if (ImGui::MenuItem("Save", SHORTCUT "S", nullptr, dirty)) { saveFile(); }
				if (ImGui::MenuItem("Save As...")) { saveAsFile(); }
			}

			ImGui::Separator();
			if (ImGui::MenuItem("Close", SHORTCUT "W")) {
				if (dirty) {
					confirmClose = true;

				} else {
					closeFile();
				}
			}

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
			if (ImGui::MenuItem("Copy", SHORTCUT "C", nullptr, editor.HasSelection())) { editor.Copy(); }
			if (ImGui::MenuItem("Cut", SHORTCUT "X", nullptr, editor.HasSelection())) { editor.Cut(); }
			if (ImGui::MenuItem("Paste", SHORTCUT "V", nullptr, ImGui::GetClipboardText() != nullptr)) { editor.Paste(); }

			ImGui::Separator();
			if (ImGui::MenuItem("Select All", SHORTCUT "A", nullptr, editor.GetText().size() != 0)) { editor.SelectAll(); }
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Program")) {
			if (ImGui::MenuItem("Build", SHORTCUT "B")) { compileFile(); }
			if (ImGui::MenuItem("Run", SHORTCUT "R", nullptr, runnable)) { runFile(); }
			ImGui::Separator();
			if (ImGui::MenuItem("Clear Error")) { clearError(); }
			if (ImGui::MenuItem("Clear Console")) { OtConsoleClass::instance()->clear(); }

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	// create the editor
	editor.Render("TextEditor");

	// scroll to line if required
	// (this has to be done here as the editor doesn't handle this well on open)
	if (scrollToLine) {
		editor.SetCursorPosition(TextEditor::Coordinates(scrollToLine - 1, 0));
		scrollToLine = 0;
	}

	// handle "close" confirmation (when user closes editor without saving)
	if (confirmClose) {
		ImGui::OpenPopup("Delete?");
		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5, 0.5));

		if (ImGui::BeginPopupModal("Delete?", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::Text("This file has changed!\nDo you really want to delete it?\n\n");
			ImGui::Separator();

			if (ImGui::Button("OK", ImVec2(120, 0))) {
				OtWorkspaceClass::instance()->closeEditor(cast<OtEditorClass>());
				ImGui::CloseCurrentPopup();
				confirmClose = false;
			}

			ImGui::SetItemDefaultFocus();
			ImGui::SameLine();

			if (ImGui::Button("Cancel", ImVec2(120, 0))) {
				ImGui::CloseCurrentPopup();
				confirmClose = false;
			}

			ImGui::EndPopup();
		}
	}

	// handle "save as" dialog
	ImVec2 maxSize = ImGui::GetIO().DisplaySize;
	ImVec2 minSize = ImVec2(maxSize.x * 0.6, maxSize.y * 0.6);

	if (ImGuiFileDialog::Instance()->Display(id + "-saveas", ImGuiWindowFlags_NoCollapse, minSize, maxSize)) {
		// open selected file if required
		if (ImGuiFileDialog::Instance()->IsOk()) {
			filename = ImGuiFileDialog::Instance()->GetFilePathName();
			saveFile();
		}

		// close dialog
		ImGuiFileDialog::Instance()->Close();

	}

	ImGui::PopID();
	ImGui::EndChild();
}

//
//	OtObjectTalkEditorClass::loadFile
//

void OtObjectTalkEditorClass::loadFile(const std::string& name) {
	// load text from file
	std::stringstream buffer;
	filename = name;

	try {
		std::ifstream stream(filename.c_str());

		if (stream.fail()) {
			OtExcept("Can't read from file [%s]", filename.c_str());
		}

		buffer << stream.rdbuf();
		stream.close();

	} catch (std::exception& e) {
		OtExcept("Can't read from file [%s], error: %s", filename.c_str(), e.what());
	}

	editor.SetText(buffer.str());
	version = editor.GetUndoCount();
}


//
//	OtObjectTalkEditorClass::newFile
//

void OtObjectTalkEditorClass::newFile() {
	OtWorkspaceClass::instance()->newFile();
}


//
//	OtObjectTalkEditorClass::openFile
//

void OtObjectTalkEditorClass::openFile() {
	OtWorkspaceClass::instance()->openFile();
}


//
//	OtObjectTalkEditorClass::saveFile
//

void OtObjectTalkEditorClass::saveFile() {
	try {
		// write text to file
		std::ofstream stream(filename.c_str());

		if (stream.fail()) {
			OtExcept("Can't write to file [%s]", filename.c_str());
		}

		stream << editor.GetText();
		stream.close();

	} catch (std::exception& e) {
		OtExcept("Can't write to file [%s], error: %s", filename.c_str(), e.what());
	}

	// reset current version number (marking the contents as clean)
	version = editor.GetUndoCount();
}


//
//	OtObjectTalkEditorClass::saveAsFile
//

void OtObjectTalkEditorClass::saveAsFile() {
	// open file save dialog
	if (!ImGuiFileDialog::Instance()->IsOpened()) {
		ImGui::PushID(this);

		ImGuiFileDialog::Instance()->OpenDialog(
			id + "-saveas",
			"Save File as...",
			".ot",
			filename,
			1,
			nullptr,
			ImGuiFileDialogFlags_Modal |
				ImGuiFileDialogFlags_DontShowHiddenFiles |
				ImGuiFileDialogFlags_ConfirmOverwrite);

		ImGui::PopID();
	}
}


//
//	OtObjectTalkEditorClass::closeFile
//

void OtObjectTalkEditorClass::closeFile() {
	OtWorkspaceClass::instance()->closeEditor(cast<OtEditorClass>());
}


//
//	OtObjectTalkEditorClass::compileFile
//

void OtObjectTalkEditorClass::compileFile() {
}


//
//	OtObjectTalkEditorClass::runFile
//

void OtObjectTalkEditorClass::runFile() {
	// reset errors
	clearError();

	// let the script runner do its job
	OtScriptRunnerClass::instance()->run(filename);
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

OtObjectTalkEditor OtObjectTalkEditorClass::create() {
	return std::make_shared<OtObjectTalkEditorClass>();
}


//
//	OtObjectTalkEditorClass::create
//

OtObjectTalkEditor OtObjectTalkEditorClass::create(const std::string& filename) {
	OtObjectTalkEditor editor = std::make_shared<OtObjectTalkEditorClass>();
	editor->loadFile(filename);
	return editor;
}
