//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <filesystem>

#include "imgui.h"

#include "OtMessageBus.h"
#include "OtUi.h"

#include "OtEditor.h"


//
//	OtEditor::render
//

void OtEditor::render(bool active) {
	// render the menu and the editor
	ImGui::BeginChild("editor", ImVec2(), ImGuiChildFlags_Border, ImGuiWindowFlags_MenuBar);
	renderMenu();
	renderEditor(active);
	ImGui::EndChild();
}


//
//	OtEditor::setFilePath
//

void OtEditor::setFilePath(const std::filesystem::path& p) {
	if (p.extension() == getFileExtension()) {
		path = p;

	} else {
		path = p;
		path.replace_extension(std::filesystem::path(getFileExtension()));
	}
}


//
//	OtEditor::fileExists
//

bool OtEditor::fileExists() {
	return std::filesystem::is_regular_file(path);
}


//
//	OtEditor::renderFileMenu
//

void OtEditor::renderFileMenu() {
	if (ImGui::BeginMenu("File")) {
		if (ImGui::MenuItem("New", OT_UI_SHORTCUT "N")) { OtMessageBus::instance()->send("new"); }
		if (ImGui::MenuItem("Open...", OT_UI_SHORTCUT "O")) { OtMessageBus::instance()->send("open"); }
		ImGui::Separator();

		if (fileExists()) {
			if (ImGui::MenuItem("Save", OT_UI_SHORTCUT "S", nullptr, isDirty())) { OtMessageBus::instance()->send("save"); }
			if (ImGui::MenuItem("Save As...")) { OtMessageBus::instance()->send("saveas"); }

		} else {
			if (ImGui::MenuItem("Save As...", OT_UI_SHORTCUT "S", nullptr, isDirty())) { OtMessageBus::instance()->send("saveas"); }
		}

		ImGui::Separator();
		if (ImGui::MenuItem("Run", OT_UI_SHORTCUT "R", nullptr, !isDirty() && fileExists())) { OtMessageBus::instance()->send("run"); }

		ImGui::Separator();
		if (ImGui::MenuItem("Close", OT_UI_SHORTCUT "W")) { OtMessageBus::instance()->send("close"); }

		ImGui::EndMenu();
	}

	// handle shortcuts
	if (ImGui::IsKeyDown(ImGuiMod_Shortcut)) {
		bool popupOpen = ImGui::IsPopupOpen("", ImGuiPopupFlags_AnyPopupId | ImGuiPopupFlags_AnyPopupLevel);

		if (ImGui::IsKeyPressed(ImGuiKey_N) && !popupOpen) {
			OtMessageBus::instance()->send("new");

		} else if (ImGui::IsKeyPressed(ImGuiKey_O) && !popupOpen) {
			OtMessageBus::instance()->send("open");

		} else if (ImGui::IsKeyPressed(ImGuiKey_S) && !popupOpen) {
			if (isDirty()) {
				if (fileExists()) {
					OtMessageBus::instance()->send("save");

				} else {
					OtMessageBus::instance()->send("saveas");
				}
			}

		} else if (ImGui::IsKeyPressed(ImGuiKey_R) && !popupOpen) {
			if (!isDirty() && fileExists()) {
				OtMessageBus::instance()->send("run");
			}

		} else if (ImGui::IsKeyPressed(ImGuiKey_W) && !popupOpen) {
			OtMessageBus::instance()->send("close");
		}
	}
}


//
//	OtEditor::renderCommonViewMenuItems
//

void OtEditor::renderCommonViewMenuItems() {
	if (ImGui::MenuItem("Toggle Console")) { OtMessageBus::instance()->send("toggleconsole"); }
	ImGui::Separator();
}
