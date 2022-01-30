//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <fstream>
#include <iostream>
#include <sstream>

#include "ImGuiFileDialog.h"

#include "OtNumbers.h"

#include "OtFramework.h"

#include "OtEditor.h"
#include "OtIdeLanguage.h"
#include "OtWorkspace.h"


//
//	OtEditorClass::render
//

void OtEditorClass::render() {
	// calculate size and position for editor window
	OtFramework framework = OtFrameworkClass::instance();
	int width = framework->getWidth();
	int height = framework->getHeight();
	ImVec2 size = ImVec2(width * 0.5, height * 0.5);

	ImVec2 pos = ImVec2(
		width * 0.05 + OtRandom(width * 0.9 - size.x),
		height * 0.05 + OtRandom(height * 0.9 - size.y));

	// determine status
	bool dirty = editor.GetUndoCount() != version;
	bool runnable = OtWorkspaceClass::instance()->canRunFile() && !dirty && filename != "untitled";

	// determine window title (while keeping constant window ID)
	std::string title = (dirty ? filename + " *" : filename) + "###" + id;

	// create the window
	ImGui::Begin(title.c_str(), nullptr, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_MenuBar);
	ImGui::SetWindowSize(size, ImGuiCond_FirstUseEver);
	ImGui::SetWindowPos(pos, ImGuiCond_FirstUseEver);
	ImGui::PushID(this);

	// set focus if required
	if (focus) {
		ImGui::SetWindowFocus();
		focus = false;
	}

	// only handle shortcuts if we have the focus
	if (ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows)) {
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
			if (dirty && filename != "untitled" && ImGui::IsKeyPressed('S')) {
				saveFile();

			} else if (ImGui::IsKeyPressed('W')) {
				if (dirty) {
					confirmClose = true;

				} else {
					closeFile();
				}

			} else if (ImGui::IsKeyPressed('B')) {
				compileFile();

			} else if (ImGui::IsKeyPressed('R')) {
				if (runnable) {
					runFile();
				}
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
			if (ImGui::MenuItem("Save", SHORTCUT "S", nullptr, dirty && filename != "untitled")) { saveFile(); }
			if (ImGui::MenuItem("Save As...")) { saveAsFile(); }

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
	ImVec2 maxSize = ImVec2(width, height);
	ImVec2 minSize = ImVec2(width * 0.6, height * 0.6);

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
	ImGui::End();
}


//
//	OtEditorClass::loadFile
//

void OtEditorClass::loadFile() {
	// load text from file
	std::stringstream buffer;

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
//	OtEditorClass::newFile
//

void OtEditorClass::newFile() {
	OtWorkspaceClass::instance()->newFile();
}


//
//	OtEditorClass::openFile
//

void OtEditorClass::openFile() {
	OtWorkspaceClass::instance()->openFile();
}


//
//	OtEditorClass::saveFile
//

void OtEditorClass::saveFile() {
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
//	OtEditorClass::saveAsFile
//

void OtEditorClass::saveAsFile() {
	// open file save dialog
	if (!ImGuiFileDialog::Instance()->IsOpened()) {
		ImGui::PushID(this);

		ImGuiFileDialog::Instance()->OpenModal(
			id + "-saveas",
			"Save File as...",
			".ot",
			filename,
			1,
			nullptr,
			ImGuiFileDialogFlags_DontShowHiddenFiles |
				ImGuiFileDialogFlags_ConfirmOverwrite);

		ImGui::PopID();
	}
}


//
//	OtEditorClass::closeFile
//

void OtEditorClass::closeFile() {
	OtWorkspaceClass::instance()->closeEditor(cast<OtEditorClass>());
}


//
//	OtEditorClass::compileFile
//

void OtEditorClass::compileFile() {
}


//
//	OtEditorClass::runFile
//

void OtEditorClass::runFile() {
	// reset errors
	clearError();

	// ask workspace to run the file
	OtWorkspaceClass::instance()->runFile(filename);
}


//
//	OtEditorClass::isDirty
//

bool OtEditorClass::isDirty() {
	return editor.GetUndoCount() != version;
}


//
//	OtEditorClass::highlightError
//

void OtEditorClass::highlightError(size_t line, const std::string error) {
	TextEditor::ErrorMarkers markers;
	markers[line] = error;
	editor.SetErrorMarkers(markers);
	scrollToLine = line;
	focus = true;
}


//
//	OtEditorClass::clearError
//

void OtEditorClass::clearError() {
	TextEditor::ErrorMarkers markers;
	editor.SetErrorMarkers(markers);
}


//
//	OtEditorClass::getMeta
//

OtType OtEditorClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtEditorClass>("Editor", OtIdeClass::getMeta());
	}

	return type;
}


//
//	OtEditorClass::create
//

OtEditor OtEditorClass::create() {
	OtEditor editor = std::make_shared<OtEditorClass>();
	editor->setType(getMeta());

	// configure editor
	editor->editor.SetLanguageDefinition(OtIdeLanguageGetDefinition());
	editor->editor.SetShowWhitespaces(false);
	editor->filename = "untitled";

	// determine window ID
	static int nextID = 1;
	std::ostringstream name;
	name << "editor" << nextID++;
	editor->id = name.str();

	// return new editor
	return editor;
}


//
//	OtEditorClass::create
//

OtEditor OtEditorClass::create(const std::string& filename) {
	OtEditor editor = create();
	editor->filename = filename;
	editor->loadFile();
	return editor;
}
