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

#include "OtFormat.h"

#include "OtFramework.h"

#include "OtLogo.h"
#include "OtObjectTalkEditor.h"
#include "OtSceneEditor.h"
#include "OtWorkspace.h"


//
//	OtWorkspaceClass::run
//

void OtWorkspaceClass::run() {
	OtFramework framework = OtFrameworkClass::instance();

	// clear all editor references on exit so we release resources at the right time
	framework->atexit([this]() {
		editors.clear();
		activeEditor = nullptr;
		activateEditorTab = nullptr;
	});

	// run the IDE workspace as an app
	framework->addCustomer(this);
	framework->run();
	framework->removeCustomer(this);
}


//
//	OtWorkspaceClass::newFile
//

void OtWorkspaceClass::newFile() {
	state = newFileState;
}


//
//	OtWorkspaceClass::getUntitledName
//

std::string OtWorkspaceClass::getUntitledName() {
	static int seqno = 1;
	std::string name;

	while (!name.size()) {
		std::string temp = OtFormat("untitled%d", seqno++);

		if (!findEditor(temp)) {
			name = temp;
		}
	}

	return name;
}


//
//	OtWorkspaceClass::newScript
//

void OtWorkspaceClass::newScript() {
	editors.push_back(OtObjectTalkEditorClass::create(getUntitledName()));
	state = editState;
}


//
//	OtWorkspaceClass::newScene
//

void OtWorkspaceClass::newScene() {
	editors.push_back(OtSceneEditorClass::create(getUntitledName()));
	state = editState;
}


//
//	OtWorkspaceClass::openFile
//

void OtWorkspaceClass::openFile() {
	ImGuiFileDialog::Instance()->OpenDialog(
		"workspace-open",
		"Select File to Open...",
		".*",
		getCWD().string(),
		1,
		nullptr,
		ImGuiFileDialogFlags_Modal |
			ImGuiFileDialogFlags_DontShowHiddenFiles |
			ImGuiFileDialogFlags_ReadOnlyFileNameField);

	state = openFileState;
}


//
//	OtWorkspaceClass::openFile
//

void OtWorkspaceClass::openFile(const std::filesystem::path& path) {
	OtEditor editor;

	// don't reopen if it is already open
	if ((editor = findEditor(path)) == nullptr) {
		// get file extension to determine editor type
		auto extension = path.extension();

		// open correct editor
		if (extension == ".ot") {
			editor = OtObjectTalkEditorClass::create(path);
			editors.push_back(editor);
			state = editState;

		} else if (extension == ".ots") {
			editor = OtSceneEditorClass::create(path);
			editors.push_back(editor);
			state = editState;

		} else {
			state = confirmErrorState;
			errorMessage = OtFormat("Can't open file with extension: %s", extension.c_str());
		}

	} else {
		// editor already exists, just active it
		activateEditor(editor);
	}

	// save the file's path as the current working directory
	cwd = path.parent_path();
}


//
//	OtWorkspaceClass::saveFile
//

void OtWorkspaceClass::saveFile() {
	activeEditor->save();
}


//
//	OtWorkspaceClass::saveAsFile
//

void OtWorkspaceClass::saveAsFile() {
	std::filesystem::path path;

	if (activeEditor->fileExists()) {
		path = std::filesystem::path(activeEditor->getFileName()).parent_path();

	} else {
		path = getCWD();
	}

	ImGuiFileDialog::Instance()->OpenDialog(
		"workspace-saveas",
		"Save File as...",
		activeEditor->getFileExtension().c_str(),
		path.string(),
		activeEditor->getShortName(),
		1,
		nullptr,
		ImGuiFileDialogFlags_Modal |
			ImGuiFileDialogFlags_DontShowHiddenFiles |
			ImGuiFileDialogFlags_ConfirmOverwrite);

	state = saveFileAsState;
}


//
//	OtWorkspaceClass::closeFile
//

void OtWorkspaceClass::closeFile() {
	// see if editor is dirty
	if (activeEditor->isDirty()) {
		state = confirmCloseState;

	} else {
		deleteEditor(activeEditor);
	}
}


//
//	OtWorkspaceClass::runFile
//

void OtWorkspaceClass::runFile() {
	// clear the console
	console.clear();

	// compose the argument array
	std::vector<std::string> args;
	currentRunnable = activeEditor->getFileName();
	args.push_back(currentRunnable.string());

	// launch a subprocess
	subprocess.start(
		getExecutablePath(),
		args,
		[this](int64_t status, int signal) {
			if (status || signal != 0) {
				console.writeError(OtFormat("\n[%s] terminated with status %d and signal %d", currentRunnable.c_str(), status, signal));

			} else {
				console.writeHelp(OtFormat("\n[%s] terminated normally", currentRunnable.c_str()));
			}
		},

		[this](const std::string& text) {
			console.write(text);
		},

		[this](const std::string& text) {
			console.writeError(text);
		});

	// show the console
	consoleFullScreen = true;
}


//
//	OtWorkspaceClass::deleteEditor
//

void OtWorkspaceClass::deleteEditor(OtEditor editor) {
	// remove specified editor from list
	editors.erase(std::remove_if(editors.begin(), editors.end(), [this] (OtEditor candidate) {
		return candidate == activeEditor;
	}), editors.end());

	if (editors.size() == 0) {
		state = splashState;
	}
}


//
//	OtWorkspaceClass::findEditor
//

OtEditor OtWorkspaceClass::findEditor(const std::filesystem::path& path) {
	for (auto& editor : editors) {
		if (editor->getFileName() == path.string() || editor->getShortName() == path.string()) {
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
//	OtWorkspaceClass::onRender
//

void OtWorkspaceClass::onRender() {
	// show the "control bar" and the console if required
	if (consoleFullScreen) {
		renderSubProcess();

	} else {
		// show splash screen if we have no editors open
		if (editors.size() == 0) {
			renderSplashScreen();
			activeEditor = nullptr;

		} else {
			// render all editors as tabs
			renderEditors();
		}

		// render any dialog boxes
		if (state == newFileState) {
			renderNewFileType();

		} else if (state == openFileState) {
			renderFileOpen();

		} else if (state == saveFileAsState) {
			renderSaveAs();

		} else if (state == confirmCloseState) {
			renderConfirmClose();

		} else if (state == confirmQuitState) {
			renderConfirmQuit();

		} else if (state == confirmErrorState) {
			renderConfirmError();
		}

		// get keyboard state to handle keyboard shortcuts
		ImGuiIO& io = ImGui::GetIO();
		auto isOSX = io.ConfigMacOSXBehaviors;
		auto alt = io.KeyAlt;
		auto ctrl = io.KeyCtrl;
		auto shift = io.KeyShift;
		auto super = io.KeySuper;
		auto isShortcut = (isOSX ? (super && !ctrl) : (ctrl && !super)) && !alt && !shift;

		// handle shortcuts based on state
		if (isShortcut) {
			if (ImGui::IsKeyPressed(ImGuiKey_N) && (state == splashState || state == editState)) {
				newFile();

			} else if (ImGui::IsKeyPressed(ImGuiKey_O) && (state == splashState || state == editState)) {
				openFile();

			} else if (ImGui::IsKeyPressed(ImGuiKey_S) && (state == editState)) {
				if (activeEditor->isDirty()) {
					if (activeEditor->fileExists()) {
						saveFile();

					} else {
						saveAsFile();
					}
				}

			} else if (ImGui::IsKeyPressed(ImGuiKey_W) && (state == editState)) {
				closeFile();
			}
		}
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
	// load logo (if required)
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
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoBringToFrontOnFocus |
			ImGuiWindowFlags_NoInputs);

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

	ImGui::Begin(
		"Workspace",
		nullptr,
		ImGuiWindowFlags_NoDecoration |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoBringToFrontOnFocus);

	// start a tab bar
	if (ImGui::BeginTabBar("Tabs", ImGuiTabBarFlags_AutoSelectNewTabs)) {
		// make clone of editor list since renderers might change it
		std::vector<OtEditor> clone = editors;

		// render all editors as tabs
		for (auto& editor : clone) {
			// determine flags for tab
			ImGuiTabItemFlags flags = 0;

			if (activateEditorTab == editor) {
				flags |= ImGuiTabItemFlags_SetSelected;
				activateEditorTab = nullptr;
			}

			if (editor->isDirty()) {
				flags |= ImGuiTabItemFlags_UnsavedDocument;
			}

			// create tab and editor
			ImGui::PushID(this);

			if (ImGui::BeginTabItem(editor->getShortName().c_str(), nullptr, flags)) {
				editor->render();
				activeEditor = editor;
				ImGui::EndTabItem();
			}

			ImGui::PopID();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();
	ImGui::PopStyleVar();
}


//
//	OtWorkspaceClass::renderNewFileType
//

void OtWorkspaceClass::renderNewFileType() {
	ImGui::OpenPopup("New File...");
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5, 0.5));

	if (ImGui::BeginPopupModal("New File...", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Text("Please select a file type:");
		ImGui::Separator();

		if (ImGui::Button("Script", ImVec2(120, 0))) {
			// create a new text editor
			newScript();
			ImGui::CloseCurrentPopup();
		}

		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();

		if (ImGui::Button("Scene", ImVec2(120, 0))) {
			newScene();
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();

		if (ImGui::Button("Cancel", ImVec2(120, 0)) || ImGui::IsKeyPressed(ImGuiKey_Escape, false)) {
			state = editors.size() ? editState : splashState;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
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

			if (state != confirmErrorState) {
				state = editState;
			}

		} else {
			state = editors.size() ? editState : splashState;
		}

		// close dialog
		ImGuiFileDialog::Instance()->Close();
	}
}


//
//	OtWorkspaceClass::renderSaveAs
//

void OtWorkspaceClass::renderSaveAs() {
	// handle saveas dialog
	ImVec2 maxSize = ImGui::GetIO().DisplaySize;
	ImVec2 minSize = ImVec2(maxSize.x * 0.5, maxSize.y * 0.5);

	if (ImGuiFileDialog::Instance()->Display("workspace-saveas", ImGuiWindowFlags_NoCollapse, minSize, maxSize)) {
		// open selected file if required
		if (ImGuiFileDialog::Instance()->IsOk()) {
			activeEditor->setFilePath(ImGuiFileDialog::Instance()->GetFilePathName());
			activeEditor->save();
			state = editState;

		} else {
			state = editors.size() ? editState : splashState;
		}

		// close dialog
		ImGuiFileDialog::Instance()->Close();
	}
}


//
//	OtWorkspaceClass::renderConfirmClose
//

void OtWorkspaceClass::renderConfirmClose() {
	ImGui::OpenPopup("Delete?");
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5, 0.5));

	if (ImGui::BeginPopupModal("Delete?", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Text("This file has changed!\nDo you really want to delete it?\n\n");
		ImGui::Separator();

		if (ImGui::Button("OK", ImVec2(120, 0))) {
			deleteEditor(activeEditor);
			ImGui::CloseCurrentPopup();
		}

		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();

		if (ImGui::Button("Cancel", ImVec2(120, 0)) || ImGui::IsKeyPressed(ImGuiKey_Escape, false)) {
			state = editState;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}


//
//	OtWorkspaceClass::renderConfirmQuit
//

void OtWorkspaceClass::renderConfirmQuit() {
	ImGui::OpenPopup("Quit Application?");
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5, 0.5));

	if (ImGui::BeginPopupModal("Quit Application?", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Text("You have unsaved files!\nDo you really want to quit?\n\n");
		ImGui::Separator();

		if (ImGui::Button("OK", ImVec2(120, 0))) {
			OtFrameworkClass::instance()->stop();
			state = editState;
			ImGui::CloseCurrentPopup();
		}

		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();

		if (ImGui::Button("Cancel", ImVec2(120, 0)) || ImGui::IsKeyPressed(ImGuiKey_Escape, false)) {
			state = editState;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}


//
//	OtWorkspaceClass::renderConfirmError
//

void OtWorkspaceClass::renderConfirmError() {
	ImGui::OpenPopup("Error");
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5, 0.5));

	if (ImGui::BeginPopupModal("Error", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Text("%s\n", errorMessage.c_str());
		ImGui::Separator();

		if (ImGui::Button("OK", ImVec2(120, 0)) || ImGui::IsKeyPressed(ImGuiKey_Escape, false)) {
			state = editors.size() ? editState : splashState;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}


//
//	OtWorkspaceClass::renderSubProcess
//

void OtWorkspaceClass::renderSubProcess() {
	// create console window
	ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
	ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

	ImGui::Begin(
		"SubProcess",
		nullptr,
		ImGuiWindowFlags_NoDecoration |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoBringToFrontOnFocus);

	// render the subprocess control bar
	std::string title = OtFormat("Runnning [%s]...", currentRunnable.c_str());
	ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "%s", title.c_str());
	ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 150.0f);

	if (subprocess.isRunning()) {
		if (ImGui::Button("Kill Process")) {
			subprocess.kill(SIGINT);
		}

	} else {
		if (ImGui::Button("Close Console", ImVec2(150.0f, 0.0f))) {
			consoleFullScreen = false;
		}
	}

	// render the console
	console.render();

	ImGui::End();
	ImGui::PopStyleVar();
}


//
//	OtWorkspaceClass::onCanQuit
//

bool OtWorkspaceClass::onCanQuit() {
	// are we currently running something?
	if (subprocess.isRunning()) {
		// then we can't quit
		return false;

	// are we showing a dialog box that we shouldn't quit?
	} else if (state == saveFileAsState || state == confirmCloseState || state == confirmQuitState) {
		// then we can't quit
		return false;

	// we can't quit if we still have a "dirty" editor
	} else {
		for (auto& editor : editors) {
			if (editor->isDirty()) {
				state = confirmQuitState;
				return false;
			}
		}

		// nothing stops us now from quiting
		return true;
	}
}


//
//	OtWorkspaceClass::getExecutablePath
//

std::filesystem::path OtWorkspaceClass::getExecutablePath() {
	// figure out where we live
	char buffer[1024];
	size_t length = 1024;
	auto status = uv_exepath(buffer, &length);
	UV_CHECK_ERROR("uv_exepath", status);
	buffer[length] = 0;
	return std::filesystem::canonical(std::filesystem::path(buffer));
}


//
//	OtWorkspaceClass::getDefaultDirectory
//

std::filesystem::path OtWorkspaceClass::getDefaultDirectory() {
	// see if we are development mode
	auto exec = getExecutablePath();
	auto root = exec.parent_path().parent_path();
	auto examples = root.parent_path() / "examples";

	// start with examples folder if we are
	if (std::filesystem::is_directory(examples)) {
		return examples;

	} else {
		// just start with user's home directory
		char buffer[1024];
		size_t length = 1024;
		auto status = uv_os_homedir(buffer, &length);
		UV_CHECK_ERROR("uv_os_homedir", status);
		std::string home(buffer, length);
		return std::filesystem::canonical(std::string(buffer, length));
	}
}


//
//	OtWorkspaceClass::getCWD
//

std::filesystem::path OtWorkspaceClass::getCWD() {
	// return our working directory
	if (cwd.empty()) {
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
