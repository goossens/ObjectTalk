//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "editor.h"
#include "language.h"


//
//	OtEditorClass::render
//

void OtEditorClass::render() {
	auto cpos = editor.GetCursorPosition();

	ImGui::Begin(filename.c_str(), nullptr, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_MenuBar);
	ImGui::SetWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);

	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("New")) {
			}

			if (ImGui::MenuItem("Open...")) {
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Save")) {
			}

			if (ImGui::MenuItem("Save As...")) {
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit")) {
			if (ImGui::MenuItem("Undo", "ALT-Backspace", nullptr, editor.CanUndo()))
				editor.Undo();

			if (ImGui::MenuItem("Redo", "Ctrl-Y", nullptr, editor.CanRedo()))
				editor.Redo();

			ImGui::Separator();

			if (ImGui::MenuItem("Copy", "Ctrl-C", nullptr, editor.HasSelection()))
				editor.Copy();

			if (ImGui::MenuItem("Cut", "Ctrl-X", nullptr, editor.HasSelection()))
				editor.Cut();

			if (ImGui::MenuItem("Delete", "Del", nullptr, editor.HasSelection()))
				editor.Delete();

			if (ImGui::MenuItem("Paste", "Ctrl-V", nullptr, ImGui::GetClipboardText() != nullptr))
				editor.Paste();

			ImGui::Separator();

			if (ImGui::MenuItem("Select all", nullptr, nullptr))
				editor.SetSelection(TextEditor::Coordinates(), TextEditor::Coordinates(editor.GetTotalLines(), 0));

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	editor.Render("TextEditor");
	ImGui::End();
}


//
//	OtEditorClass::getMeta
//

OtType OtEditorClass::getMeta() {
	static OtType type = nullptr;

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
	editor->editor.SetLanguageDefinition(OtLanguageGetDefinition());
	editor->editor.SetShowWhitespaces(false);
	editor->filename = "untitled";
	return editor;
}



//
//	OtEditorClass::create
//

OtEditor OtEditorClass::create(const std::string& filename) {
	OtEditor editor = create();
	editor->filename = filename;
	return editor;
}
