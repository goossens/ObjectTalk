//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"

#include "OtMessageBus.h"
#include "OtPathTools.h"
#include "OtUi.h"

#include "OtEditor.h"


//
//	OtEditor::renderFileMenu
//

void OtEditor::renderFileMenu() {
	if (ImGui::BeginMenu("File")) {
		if (ImGui::MenuItem("New", OT_UI_SHORTCUT "N")) { OtMessageBus::instance()->send("new"); }
		if (ImGui::MenuItem("Open...", OT_UI_SHORTCUT "O")) { OtMessageBus::instance()->send("open"); }
		ImGui::Separator();

		if (OtPathIsRegularFile(getFilePath())) {
			if (ImGui::MenuItem("Save", OT_UI_SHORTCUT "S", nullptr, isDirty())) { OtMessageBus::instance()->send("save"); }
			if (ImGui::MenuItem("Save As...")) { OtMessageBus::instance()->send("saveas"); }

		} else {
			if (ImGui::MenuItem("Save As...", OT_UI_SHORTCUT "S", nullptr, isDirty())) { OtMessageBus::instance()->send("saveas"); }
		}

		ImGui::Separator();
		if (ImGui::MenuItem("Run", OT_UI_SHORTCUT "R", nullptr, !isDirty() && OtPathIsRegularFile(getFilePath()))) { OtMessageBus::instance()->send("run"); }

		ImGui::Separator();
		if (ImGui::MenuItem("Close", OT_UI_SHORTCUT "W")) { OtMessageBus::instance()->send("close"); }

		ImGui::EndMenu();
	}

	// handle shortcuts
	if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) && ImGui::IsKeyDown(ImGuiMod_Shortcut)) {
		if (ImGui::IsKeyPressed(ImGuiKey_N)) {
			OtMessageBus::instance()->send("new");

		} else if (ImGui::IsKeyPressed(ImGuiKey_O)) {
			OtMessageBus::instance()->send("open");

		} else if (ImGui::IsKeyPressed(ImGuiKey_S)) {
			if (isDirty()) {
				if (OtPathIsRegularFile(getFilePath())) {
					OtMessageBus::instance()->send("save");

				} else {
					OtMessageBus::instance()->send("saveas");
				}
			}

		} else if (ImGui::IsKeyPressed(ImGuiKey_R)) {
			if (!isDirty() && OtPathIsRegularFile(getFilePath())) {
				OtMessageBus::instance()->send("run");
			}

		} else if (ImGui::IsKeyPressed(ImGuiKey_W)) {
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
