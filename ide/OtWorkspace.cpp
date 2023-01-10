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
#include "ImGuiFileDialog.h"

#include "OtLibuv.h"
#include "OtOS.h"

#include "OtFramework.h"

#include "OtConsole.h"
#include "OtLogo.h"
#include "OtObjectTalkEditor.h"
#include "OtScriptRunner.h"
#include "OtWorkspace.h"


//
//	OtWorkspaceClass::run
//

void OtWorkspaceClass::run() {
	// get access to framework
	OtFramework framework = OtFrameworkClass::instance();

	// add ourselves to the GUI framework as a customer
	framework->addCustomer(this);

	// run the GUI framework
	framework->run();

	// we're done
	framework->removeCustomer(this);
}


//
//	OtWorkspaceClass::newFile
//

void OtWorkspaceClass::newFile() {
	// don't create new files when we're in the process of opening one
	if (!ImGuiFileDialog::Instance()->IsOpened()) {
		editors.push_back(OtObjectTalkEditorClass::create());
	}
}


//
//	OtWorkspaceClass::openFile
//

void OtWorkspaceClass::openFile() {
	// open file select dialog (if one isn't open already)
	if (!ImGuiFileDialog::Instance()->IsOpened()) {
		ImGuiFileDialog::Instance()->OpenDialog(
			"workspace-open",
			"Select File to Open...",
			".ot",
			getCWD(),
			1,
			nullptr,
		   ImGuiFileDialogFlags_Modal |
				ImGuiFileDialogFlags_DontShowHiddenFiles |
				ImGuiFileDialogFlags_ReadOnlyFileNameField);
	}
}


//
//	OtWorkspaceClass::openFile
//

OtEditor OtWorkspaceClass::openFile(const std::string& filename) {
	OtEditor editor;

	// don't reopen if it is already open
	if ((editor = findEditor(filename)) != nullptr) {

	} else {
		// open a new editor
		editor = OtObjectTalkEditorClass::create(filename);
		editors.push_back(editor);

	}
	// save the file's path as the current working directory
	cwd = std::filesystem::path(filename).parent_path().string();

	return editor;
}


//
//	OtWorkspaceClass::closeEditor
//

void OtWorkspaceClass::closeEditor(OtEditor editor) {
	// remove specified editor from list
	editors.erase(std::remove_if(editors.begin(), editors.end(), [editor] (OtEditor candidate) {
		return candidate == editor;
	}), editors.end());
}


//
//	OtWorkspaceClass::findEditor
//

OtEditor OtWorkspaceClass::findEditor(const std::string& filename) {
	for (auto& editor : editors) {
		if (editor->getFileName() == filename) {
			return editor;
		}
	}

	return nullptr;
}


//
//	OtWorkspaceClass::activateEditor
//

void OtWorkspaceClass::activateEditor(OtEditor editor) {
	activateEditorTab = editor;
}


//
//	OtWorkspaceClass::onUpdate
//

void OtWorkspaceClass::onUpdate() {
	// update the console and script runner
	OtConsoleClass::instance()->update();
	OtScriptRunnerClass::instance()->update();
}


//
//	OtWorkspaceClass::onRender
//

void OtWorkspaceClass::onRender() {
	// render all editors (if required)
	if (!OtScriptRunnerClass::instance()->isRunningGUI()) {
		// create workspace (if required)
		if (editors.size()) {
			// render all editors as tabs
			renderEditors();

		} else {
			// just show splash screen
			renderSplashScreen();
		}

		renderFileOpen();
		renderQuitConfirmation();
	}
}


//
//	OtWorkspaceClass::renderSplashScreen
//

static void centerText(const char* text) {
	ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize(text).x) * 0.5f);
	ImGui::Text("%s", text);
}

void OtWorkspaceClass::renderSplashScreen() {
	// setup log (if required)
	if (!logo.isValid()) {
		// load it from file that lives in memory (to keep ot as a single file)
		logo.loadFromFileInMemory((void*) &OtLogoData, sizeof(OtLogoData));

		// ensure logo is cleared at exit to avoid a memory leak
		OtFrameworkClass::instance()->atexit([this] {
			logo.clear();
		});
	}

	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5, 0.5));

	ImGui::Begin(
		"SplashScreen",
		nullptr,
		ImGuiWindowFlags_NoDecoration |
			ImGuiWindowFlags_AlwaysAutoResize |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove);

	ImGui::Image((void*)(intptr_t) logo.getTextureIndex(), ImVec2(OtLogoWidth, OtLogoHeight));

	centerText("Welcome to the ObjectTalk");
	centerText("Integrated Development Environment (IDE)");
	centerText("");

	if (ImGui::GetIO().ConfigMacOSXBehaviors) {
		centerText("Use Cmd-N to create a new file");
		centerText("Use Cmd-O to open an existing file");

	} else {
		centerText("Use Ctrl-N to create a new file");
		centerText("Use Ctrl-O to open an existing file");
	}

	ImGui::End();
}


//
//	OtWorkspaceClass::renderEditors
//

void OtWorkspaceClass::renderEditors() {
	// create workspace window
	ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
	ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::Begin("Workspace", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize);

	// render all editors as tabs
	if (ImGui::BeginTabBar("TabBar", ImGuiTabBarFlags_AutoSelectNewTabs)) {
		// make clone of editor list since renderers might change it
		std::vector<OtEditor> clone = editors;

		for (auto& editor : clone) {
			ImGuiTabItemFlags flags = 0;

			if (activateEditorTab == editor) {
				flags = ImGuiTabItemFlags_SetSelected;
			}

			if (ImGui::BeginTabItem(editor->getShortName().c_str(), nullptr, flags)) {
				editor->render();
				ImGui::EndTabItem();
			}
		}

		activateEditorTab = nullptr;
		ImGui::EndTabBar();
	}

	ImGui::End();
	ImGui::PopStyleVar();
}


//
//	OtWorkspaceClass::renderFileOpen
//

void OtWorkspaceClass::renderFileOpen() {
	// handle file open dialog
	ImVec2 maxSize = ImGui::GetIO().DisplaySize;
	ImVec2 minSize = ImVec2(maxSize.x * 0.5, maxSize.y * 0.5);

	if (ImGuiFileDialog::Instance()->Display("workspace-open", ImGuiWindowFlags_NoCollapse, minSize, maxSize)) {
		// open selected file if required
		if (ImGuiFileDialog::Instance()->IsOk()) {
			std::map<std::string, std::string> selected = ImGuiFileDialog::Instance()->GetSelection();
			openFile(selected.begin()->second);
		}

		// close dialog
		ImGuiFileDialog::Instance()->Close();

	} else {
		// get keyboard state to handle keyboard shortcuts
		ImGuiIO& io = ImGui::GetIO();
		auto isOSX = io.ConfigMacOSXBehaviors;
		auto alt = io.KeyAlt;
		auto ctrl = io.KeyCtrl;
		auto shift = io.KeyShift;
		auto super = io.KeySuper;
		auto isShortcut = (isOSX ? (super && !ctrl) : (ctrl && !super)) && !alt && !shift;

		// handle new file shortcut
		if (isShortcut) {
			if (ImGui::IsKeyPressed(ImGuiKey_N)) {
				newFile();

			} else if (ImGui::IsKeyPressed(ImGuiKey_O)) {
				openFile();
			}
		}
	}
}


//
//	OtWorkspaceClass::renderQuitConfirmation
//

void OtWorkspaceClass::renderQuitConfirmation() {
	// handle "quit" confirmation (when user quits without saving)
	if (confirmQuit) {
		ImGui::OpenPopup("Quit Application?");
		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5, 0.5));

		if (ImGui::BeginPopupModal("Quit Application?", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::Text("You have unsaved files!\nDo you really want to quit?\n\n");
			ImGui::Separator();

			if (ImGui::Button("OK", ImVec2(120, 0))) {
				OtFrameworkClass::instance()->stop();
				ImGui::CloseCurrentPopup();
				confirmQuit = false;
			}

			ImGui::SetItemDefaultFocus();
			ImGui::SameLine();

			if (ImGui::Button("Cancel", ImVec2(120, 0))) {
				ImGui::CloseCurrentPopup();
				confirmQuit = false;
			}

			ImGui::EndPopup();
		}
	}
}


//
//	OtWorkspaceClass::onCanQuit
//

bool OtWorkspaceClass::onCanQuit() {
	auto runner = OtScriptRunnerClass::instance();
	auto os = OtOSClass::instance();

	// are we currently running a server script
	if (runner->isRunningServer()) {
		// just stop the script and keep the IDE going
		os->stopServer();
		return false;

	// are we currently running a GUI script
	} else if (runner->isRunningGUI()) {
		// just stop the script and keep the IDE going
		os->stopGUI();
		return false;

	} else {
		// we can't quit if we still have a "dirty" editor
		for (auto& editor : editors) {
			if (editor->isDirty()) {
				confirmQuit = true;
				return false;
			}
		}

		return true;
	}
}


//
//	OtWorkspaceClass::getDefaultDirectory
//

std::string OtWorkspaceClass::getDefaultDirectory() {
	// figure out where we live
	char buffer[1024];
	size_t length = 1024;
	auto status = uv_exepath(buffer, &length);
	UV_CHECK_ERROR("uv_exepath", status);
	std::string home(buffer, length);

	// see if we are development mode
	auto exec = std::filesystem::path(home);
	auto root = std::filesystem::canonical(exec).parent_path().parent_path();
	auto examples = root.parent_path() / "examples";

	// start with examples folder if we are
	if (std::filesystem::is_directory(examples)) {
		return examples.string();

	} else {
		// just start with user's home directory
		auto status = uv_os_homedir(buffer, &length);
		UV_CHECK_ERROR("uv_os_homedir", status);
		std::string home(buffer, length);
		return std::filesystem::canonical(std::string(buffer, length)).string();
	}
}


//
//	OtWorkspaceClass::getCWD
//

std::string OtWorkspaceClass::getCWD() {
	// return our working directory
	if (cwd.size() == 0) {
		cwd = getDefaultDirectory();
	}

	return cwd;
}


//
//	OtWorkspaceClass::create
//

OtWorkspace OtWorkspaceClass::create() {
	return OtWorkspaceClass::instance();
}
