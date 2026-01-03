//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "fmt/format.h"
#include "imgui.h"

#include "OtMessageBus.h"
#include "OtPath.h"
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
//	OtEditor::renderMenuBar
//


void OtEditor::renderMenuBar(bool canRun) {
	// determine if this editor's contents is "runnable"
	bool runnable = canRun && isRunnable() && !isDirty() && OtPath::isRegularFile(path);

	// create menubar
	if (ImGui::BeginMenuBar()) {
		auto width = ImGui::GetContentRegionAvail().x;

		// create file menu
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("New", OT_UI_SHORTCUT "N")) { OtMessageBus::send("new"); }
			if (ImGui::MenuItem("Open...", OT_UI_SHORTCUT "O")) { OtMessageBus::send("open"); }
			ImGui::Separator();

			if (OtPath::isRegularFile(path)) {
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

		// let editor render specific menus
		renderMenus();

		// create window menu
		if (ImGui::BeginMenu("Window")) {
			if (ImGui::MenuItem("Toggle Console")) { OtMessageBus::send("toggleconsole"); }
			ImGui::Separator();
			if (ImGui::MenuItem("Move to Window", nullptr, nullptr, isRenderedInTab())) { setVisualState(VisualState::inWindow); }
			if (ImGui::MenuItem("Move to Tabs", nullptr, nullptr, isRenderedInWindow())) { setVisualState(VisualState::inTab); }
			ImGui::Separator();
			if (ImGui::MenuItem("Move all to Windows")) { OtMessageBus::send("windowfy"); }
			if (ImGui::MenuItem("Move all to Tabs")) { OtMessageBus::send("tabify"); }

			ImGui::EndMenu();
		}

		// allow user to move editor from tab to floating window and visa versa
		ImGui::SameLine(width - ImGui::GetFrameHeight());
		ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_MenuBarBg]);

		if (ImGui::ArrowButton("toInTab", isRenderedInTab() ? ImGuiDir_Down : ImGuiDir_Up)) {
			setVisualState(isRenderedInTab() ? VisualState::inWindow : VisualState::inTab);
		}

		ImGui::PopStyleColor();
		ImGui::EndMenuBar();
	}

	// handle keyboard shortcuts
	if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) && !ImGui::GetIO().WantCaptureKeyboard) {
		if (ImGui::IsKeyDown(ImGuiMod_Ctrl)) {
			if (ImGui::IsKeyPressed(ImGuiKey_N)) {
				OtMessageBus::send("new");

			} else if (ImGui::IsKeyPressed(ImGuiKey_O)) {
				OtMessageBus::send("open");

			} else if (ImGui::IsKeyPressed(ImGuiKey_S)) {
				if (isDirty()) {
					if (OtPath::isRegularFile(path)) {
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

		// also let editor handle specific shortcuts
		handleShortcuts();
	}
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
