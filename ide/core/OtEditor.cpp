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

#include "OtEditor.h"


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
#if __APPLE__
#define SHORTCUT "Cmd-"
#else
#define SHORTCUT "Ctrl-"
#endif

	if (ImGui::BeginMenu("File")) {
		if (ImGui::MenuItem("New", SHORTCUT "N")) { OtMessageBus::instance()->send("new"); }
		if (ImGui::MenuItem("Open...", SHORTCUT "O")) { OtMessageBus::instance()->send("open"); }
		ImGui::Separator();

		if (fileExists()) {
			if (ImGui::MenuItem("Save", SHORTCUT "S", nullptr, isDirty())) { OtMessageBus::instance()->send("save"); }
			if (ImGui::MenuItem("Save As...")) { OtMessageBus::instance()->send("saveas"); }

		} else {
			if (ImGui::MenuItem("Save As...", SHORTCUT "S", nullptr, isDirty())) { OtMessageBus::instance()->send("saveas"); }
		}

		ImGui::Separator();
		if (ImGui::MenuItem("Run", SHORTCUT "R", nullptr, !isDirty() && fileExists())) { OtMessageBus::instance()->send("run"); }

		ImGui::Separator();
		if (ImGui::MenuItem("Close", SHORTCUT "W")) { OtMessageBus::instance()->send("close"); }

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
