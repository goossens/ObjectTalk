//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "fmt/format.h"
#include "imgui.h"

#include "OtMessageBus.h"
#include "OtPathTools.h"
#include "OtUi.h"

#include "OtEditor.h"


//
//	OtEditor::newFile
//

void OtEditor::newFile(const std::string& p) {
	unfollow();
	path = p;
	clear();
}


//
//	OtEditor::openFile
//

void OtEditor::openFile(const std::string& p) {
	path = p;
	load();
	follow();
}


//
//	OtEditor::saveFile
//

void OtEditor::saveFile() {
	ignoreNextFileUpdate = true;
	save();
}


//
//	OtEditor::saveAsFile
//

void OtEditor::saveAsFile(const std::string& p) {
	unfollow();
	path = p;
	save();
	follow();
}


//
//	OtEditor::follow
//

void OtEditor::follow() {
	// follow file changes
	follower.follow(path, [this](){
		if (ignoreNextFileUpdate) {
			ignoreNextFileUpdate = false;

		} else if (!isDirty()) {
			// file was not yet edited; it's safe to reload
			load();

		} else {
			// file was edited; don't reload but show message
			OtMessageBus::send(fmt::format("warning File {} was edited externally.\nBe careful when saving!", path));
		}
	});
}


//
//	OtEditor::unfollow
//

void OtEditor::unfollow() {
	follower.unfollow();
}


//
//	OtEditor::renderFileMenu
//

void OtEditor::renderFileMenu(bool canRun) {
	bool runnable = canRun && isRunnable() && !isDirty() && OtPathIsRegularFile(path);

	if (ImGui::BeginMenu("File")) {
		if (ImGui::MenuItem("New", OT_UI_SHORTCUT "N")) { OtMessageBus::send("new"); }
		if (ImGui::MenuItem("Open...", OT_UI_SHORTCUT "O")) { OtMessageBus::send("open"); }
		ImGui::Separator();

		if (OtPathIsRegularFile(path)) {
			if (ImGui::MenuItem("Save", OT_UI_SHORTCUT "S", nullptr, isDirty())) { OtMessageBus::send("save"); }
			if (ImGui::MenuItem("Save As...")) { OtMessageBus::send("saveas"); }

		} else {
			if (ImGui::MenuItem("Save As...", OT_UI_SHORTCUT "S", nullptr, isDirty())) { OtMessageBus::send("saveas"); }
		}

		ImGui::Separator();
		if (ImGui::MenuItem("Run", OT_UI_SHORTCUT "R", nullptr, runnable)) { OtMessageBus::send("run"); }

		ImGui::Separator();
		if (ImGui::MenuItem("Close", OT_UI_SHORTCUT "W")) { OtMessageBus::send("close"); }

		ImGui::EndMenu();
	}

	// handle shortcuts
	if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) && ImGui::IsKeyDown(ImGuiMod_Ctrl)) {
		if (ImGui::IsKeyPressed(ImGuiKey_N)) {
			OtMessageBus::send("new");

		} else if (ImGui::IsKeyPressed(ImGuiKey_O)) {
			OtMessageBus::send("open");

		} else if (ImGui::IsKeyPressed(ImGuiKey_S)) {
			if (isDirty()) {
				if (OtPathIsRegularFile(path)) {
					OtMessageBus::send("save");

				} else {
					OtMessageBus::send("saveas");
				}
			}

		} else if (ImGui::IsKeyPressed(ImGuiKey_R) && runnable) {
			OtMessageBus::send("run");

		} else if (ImGui::IsKeyPressed(ImGuiKey_W)) {
			OtMessageBus::send("close");
		}
	}
}
