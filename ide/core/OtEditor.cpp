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
	path = p;
	follower.clear();
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
			//file was edited; don't reload but show message
			OtMessageBus::instance()->send(fmt::format("warning File {} was edited externally.\nBe careful when saving!", path));
		}
	});
}


//
//	OtEditor::renderFileMenu
//

void OtEditor::renderFileMenu() {
	if (ImGui::BeginMenu("File")) {
		if (ImGui::MenuItem("New", OT_UI_SHORTCUT "N")) { OtMessageBus::instance()->send("new"); }
		if (ImGui::MenuItem("Open...", OT_UI_SHORTCUT "O")) { OtMessageBus::instance()->send("open"); }
		ImGui::Separator();

		if (OtPathIsRegularFile(path)) {
			if (ImGui::MenuItem("Save", OT_UI_SHORTCUT "S", nullptr, isDirty())) { OtMessageBus::instance()->send("save"); }
			if (ImGui::MenuItem("Save As...")) { OtMessageBus::instance()->send("saveas"); }

		} else {
			if (ImGui::MenuItem("Save As...", OT_UI_SHORTCUT "S", nullptr, isDirty())) { OtMessageBus::instance()->send("saveas"); }
		}

		ImGui::Separator();
		if (ImGui::MenuItem("Run", OT_UI_SHORTCUT "R", nullptr, !isDirty() && OtPathIsRegularFile(path))) { OtMessageBus::instance()->send("run"); }

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
				if (OtPathIsRegularFile(path)) {
					OtMessageBus::instance()->send("save");

				} else {
					OtMessageBus::instance()->send("saveas");
				}
			}

		} else if (ImGui::IsKeyPressed(ImGuiKey_R)) {
			if (!isDirty() && OtPathIsRegularFile(path)) {
				OtMessageBus::instance()->send("run");
			}

		} else if (ImGui::IsKeyPressed(ImGuiKey_W)) {
			OtMessageBus::instance()->send("close");
		}
	}
}
