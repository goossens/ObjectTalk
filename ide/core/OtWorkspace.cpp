//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>
#include <string>

#include "fmt/format.h"
#include "imgui.h"
#include "ImGuiFileDialog.h"

#include "OtAssert.h"
#include "OtStderrMultiplexer.h"

#include "OtMessageBus.h"
#include "OtPathTools.h"
#include "OtUi.h"

#include "OtNodesEditor.h"
#include "OtObjectTalkEditor.h"
#include "OtSceneEditor.h"
#include "OtWorkspace.h"


//
//	OtWorkspace::onSetup
//

void OtWorkspace::onSetup() {
	// set the current directory to examples if we are in development mode
	auto exec = getExecutablePath();
	auto root = OtPathGetParent(OtPathGetParent(OtPathGetParent(exec)));
	auto examples = OtPathJoin(root, "examples");

	if (OtPathIsDirectory(examples)) {
		OtPathChangeDirectory(examples);
	}

	// listen for message bus events
	OtMessageBus::instance()->listen([this](const std::string& message) {
		onMessage(message);
	});
}


//
//	OtWorkspace::onMessage
//

void OtWorkspace::onMessage(const std::string& msg) {
	// split the command
	auto delimiter = msg.find(" ");

	if (delimiter != std::string::npos) {
		auto command = msg.substr(0, delimiter);
		auto operand = msg.substr(delimiter + 1);

		// process each command
		if (command == "openintab") {
			openFile(operand, OtEditor::inTab);

		} else if (command == "openinwindow") {
			openFile(operand, OtEditor::inWindow);

		} else if (command == "warning") {
			state = confirmWarningState;
			message = operand;

		} else if (command == "error") {
			state = confirmErrorState;
			message = operand;
		}

	} else {
		// process each command
		auto command = msg;

		if (command == "new") {
			newFile();

		} else if (command == "open") {
			openFile();

		} else if (command == "save") {
			saveFile();

		} else if (command == "saveas") {
			saveAsFile();

		} else if (command == "run") {
			runFile();

		} else if (command == "close") {
			closeFile();

		} else if (command == "toggleconsole") {
			consoleAsPanel = !consoleAsPanel;
		}
	}
}


//
//	OtWorkspace::onRender
//

void OtWorkspace::onRender()
{
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

		} else if (state == confirmWarningState) {
			renderConfirmWarning();

		} else if (state == confirmErrorState) {
			renderConfirmError();
		}
	}
}


//
//	OtWorkspace::onTerminate
//

void OtWorkspace::onTerminate() {
	// clear all resource references in order to release them at the right time
	editors.clear();
	activeEditor = nullptr;
	editorToActivate = nullptr;
	logo = nullptr;
}


//
//	OtWorkspace::onCanQuit
//

bool OtWorkspace::onCanQuit() {
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
//	OtWorkspace::newFile
//

void OtWorkspace::newFile() {
	state = newFileState;
}


//
//	OtWorkspace::getUntitledName
//

std::string OtWorkspace::getUntitledName(const char* ext) {
	static int seqno = 1;
	std::string name;

	while (!name.size()) {
		std::string temp = fmt::format("untitled{}{}", seqno++, ext);

		if (!findEditor(temp)) {
			name = temp;
		}
	}

	return name;
}


//
//	OtWorkspace::newScript
//

void OtWorkspace::newScript() {
	auto editor = std::make_shared<OtObjectTalkEditor>();
	editor->newFile(getUntitledName(".ot"));
	editors.push_back(editor);
	state = editState;
}


//
//	OtWorkspace::newScene
//

void OtWorkspace::newScene() {
	auto editor = std::make_shared<OtSceneEditor>();
	editor->newFile(getUntitledName(".ots"));
	editors.push_back(editor);
	state = editState;
}


//
//	OtWorkspace::newNodes
//

void OtWorkspace::newNodes() {
	auto editor = std::make_shared<OtNodesEditor>();
	editor->newFile(getUntitledName(".otn"));
	editors.push_back(editor);
	state = editState;
}


//
//	OtWorkspace::openFile
//

void OtWorkspace::openFile() {
	IGFD::FileDialogConfig config;
	config.path = OtPathGetCurrentWorkingDirectory();
	config.countSelectionMax = 1;

	config.flags = ImGuiFileDialogFlags_Modal |
			ImGuiFileDialogFlags_DontShowHiddenFiles |
			ImGuiFileDialogFlags_ReadOnlyFileNameField;

	ImGuiFileDialog::Instance()->OpenDialog("workspace-open", "Select File to Open...", ".*", config);
	state = openFileState;
}


//
//	OtWorkspace::openFile
//

void OtWorkspace::openFile(const std::string& path, int visualState) {
	// don't reopen if it's already open
	std::shared_ptr<OtEditor> editor = findEditor(path);

	if (editor == nullptr) {
		// get file extension to determine editor type
		auto extension = OtPathGetExtension(path);

		// open correct editor
		if (extension == ".ot" || extension == ".ots" || extension == ".otn") {
			if (extension == ".ot") {
			editor = std::make_shared<OtObjectTalkEditor>();

			} else if (extension == ".ots") {
				editor = std::make_shared<OtSceneEditor>();

			} else if (extension == ".otn") {
				editor = std::make_shared<OtNodesEditor>();
			}

			editor->openFile(path);
			editor->setVisualState(visualState);
			editors.push_back(editor);
			activateEditor(editor);
			state = editState;

		} else {
			state = confirmErrorState;
			message = fmt::format("Can't open file with extension: {}", extension);
		}

	} else {
		// editor already exists, just activate it
		activateEditor(editor);
	}
}


//
//	OtWorkspace::saveFile
//

void OtWorkspace::saveFile() {
	activeEditor->saveFile();
}


//
//	OtWorkspace::saveAsFile
//

void OtWorkspace::saveAsFile() {
	IGFD::FileDialogConfig config;
	config.path = OtPathGetCurrentWorkingDirectory();
	config.countSelectionMax = 1;

	config.flags = ImGuiFileDialogFlags_Modal |
			ImGuiFileDialogFlags_DontShowHiddenFiles |
			ImGuiFileDialogFlags_ConfirmOverwrite;

	ImGuiFileDialog::Instance()->OpenDialog(
		"workspace-saveas",
		"Save File as...",
		activeEditor->getExtension().c_str(),
		config);

	state = saveFileAsState;
}


//
//	OtWorkspace::closeFile
//

void OtWorkspace::closeFile() {
	// see if editor is dirty
	if (activeEditor->isDirty()) {
		state = confirmCloseState;

	} else {
		deleteEditor(activeEditor);
	}
}


//
//	OtWorkspace::runFile
//

void OtWorkspace::runFile() {
	// clear the console and all error highlighting
	console.clear();

	for (auto editor : editors) {
		auto scriptEditor = std::dynamic_pointer_cast<OtObjectTalkEditor>(editor);

		if (scriptEditor) {
			scriptEditor->clearError();
		}
	}

	// compose the argument array
	std::vector<std::string> args;
	args.push_back("--child");
	currentRunnable = activeEditor->getPath();
	args.push_back(currentRunnable);

	// launch a subprocess
	subprocess.start(
		getExecutablePath(),
		args,
		[this](int64_t status, int signal) {
			if (status || signal != 0) {
				console.writeError(fmt::format("\n[{}] terminated with status {} and signal {}\n", currentRunnable, status, signal));

			} else {
				console.writeSuccess(fmt::format("\n[{}] terminated normally\n", currentRunnable));
			}

			consoleFullScreen = false;

			if (OtPathGetExtension(currentRunnable) != ".ots") {
				consoleAsPanel = true;
			}
		},

		[this](const std::string& text) {
			console.write(text);
		},

		[this](const std::string& text) {
			OtStderrMultiplexer::instance()->demuliplex(
				text,
				[this](const std::string& message) {
					console.writeError(message);
				},
				[this](int type, const std::string& message) {
					console.writeLog(type, message);
				},
				[this](OtException& exception) {
					highlightError(exception);
				});
		});

	// show the console
	consoleFullScreen = true;
	consoleAsPanel = false;
}


//
//	OtWorkspace::deleteEditor
//

void OtWorkspace::deleteEditor(std::shared_ptr<OtEditor> editor) {
	// remove specified editor from list
	editors.erase(std::remove_if(editors.begin(), editors.end(), [this](std::shared_ptr<OtEditor> candidate) {
		return candidate == activeEditor;
	}), editors.end());

	if (editors.size() == 0) {
		state = splashState;
		activeEditor = nullptr;
	}
}


//
//	OtWorkspace::findEditor
//

std::shared_ptr<OtEditor> OtWorkspace::findEditor(const std::string& path) {
	for (auto& editor : editors) {
		auto filePath = editor->getPath();

		if (filePath == path || OtPathGetFilename(filePath) == path) {
			return editor;
		}
	}

	return nullptr;
}


//
//	OtWorkspace::activateEditor
//

void OtWorkspace::activateEditor(std::shared_ptr<OtEditor> editor) {
	editorToActivate = editor;
}


//
//	OtWorkspace::renderSplashScreen
//

void OtWorkspace::renderSplashScreen() {
	// load logo (if required)
	if (!logo) {
		logo = std::make_shared<OtLogo>();
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

	ImGui::Image((void*)(intptr_t) logo->getTextureIndex(), ImVec2(logo->getWidth(), logo->getHeight()));

	OtUiCenteredText("Welcome to the ObjectTalk");
	OtUiCenteredText("Integrated Development Environment (IDE)");
	OtUiCenteredText("");

	if (ImGui::GetIO().ConfigMacOSXBehaviors) {
		OtUiCenteredText("Use Cmd-N to create a new file");
		OtUiCenteredText("Use Cmd-O to open an existing file");

	} else {
		OtUiCenteredText("Use Ctrl-N to create a new file");
		OtUiCenteredText("Use Ctrl-O to open an existing file");
	}

	ImGui::End();

	// handle shortcuts
	if (ImGui::IsKeyDown(ImGuiMod_Ctrl)) {
		if (ImGui::IsKeyPressed(ImGuiKey_N)) {
			newFile();

		} else if (ImGui::IsKeyPressed(ImGuiKey_O)) {
			openFile();
		}
	}
}


//
//	OtWorkspace::renderEditors
//

void OtWorkspace::renderEditors() {
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

	if (consoleAsPanel) {
		// split the screen between the editors and the console
		determinePanelHeights();
		auto spacing = ImGui::GetStyle().ItemSpacing;
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
		ImGui::BeginChild("Editors", ImVec2(0.0f, editorsHeight));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, spacing);
	}

	// make clone of editor list since renderers might change it
	std::vector<std::shared_ptr<OtEditor>> clone = editors;

	// start a tab bar and render all editors that have an "in tab" state
	if (ImGui::BeginTabBar("Tabs", ImGuiTabBarFlags_AutoSelectNewTabs)) {
		// render all editors as tabs
		for (auto& editor : clone) {
			if (editor->isRenderedInTab()) {
				// this ID is required to handle duplicate filenames
				ImGui::PushID(editor.get());

				// determine flags for tab
				ImGuiTabItemFlags flags = 0;

				if (editorToActivate == editor) {
					flags |= ImGuiTabItemFlags_SetSelected;
					editorToActivate = nullptr;
				}

				if (editor->isDirty()) {
					flags |= ImGuiTabItemFlags_UnsavedDocument;
				}

				// create tab and editor
				if (ImGui::BeginTabItem(OtPathGetFilename(editor->getPath()).c_str(), nullptr, flags)) {
					ImGui::BeginChild("editor", ImVec2(), ImGuiChildFlags_Borders, ImGuiWindowFlags_MenuBar);

					editor->renderMenu();
					editor->renderEditor();

					ImGui::EndChild();
					ImGui::EndTabItem();
					activeEditor = editor;
				}

				ImGui::PopID();
			}
		}

		ImGui::EndTabBar();
	}

	// render all editors that have an "in window" state
	for (auto& editor : clone) {
		if (editor->isRenderedInWindow()) {
			// determine window flags
			ImGuiTabItemFlags flags = ImGuiWindowFlags_MenuBar;

			if (editor->isDirty()) {
				flags |= ImGuiWindowFlags_UnsavedDocument;
			}

			// focus on this editor (if required)
			if (editorToActivate == editor) {
				ImGui::SetNextWindowFocus();
				editorToActivate = nullptr;
			}

			// set size and position when opened for the first time
			auto size = ImGui::GetIO().DisplaySize;
			ImGui::SetNextWindowPos(ImVec2(size.x / 6.0f, size.y / 6.0f), ImGuiCond_Once);
			ImGui::SetNextWindowSize(ImVec2(size.x * 0.6f, size.y * 0.6f), ImGuiCond_Once);

			// render editor in seperate window
			bool open = true;
			ImGui::Begin(OtPathGetFilename(editor->getPath()).c_str(), &open, flags);

			editor->renderMenu();
			editor->renderEditor();

			if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)) {
				activeEditor = editor;
			}

			ImGui::End();

			if (!open) {
				activeEditor = editor;
				closeFile();
			}
		}
	}

	if (consoleAsPanel) {
		// split the screen between the editors and the console
		ImGui::PopStyleVar();
		ImGui::EndChild();
		ImGui::PopStyleVar();
		OtUiSplitterVertical(&editorsHeight, editorsMinHeight, editorsMaxHeight);
		console.render();
	}

	ImGui::End();
	ImGui::PopStyleVar();
}


//
//	OtWorkspace::renderNewFileType
//

void OtWorkspace::renderNewFileType() {
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

		if (ImGui::Button("Nodes", ImVec2(120, 0))) {
			newNodes();
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
//	OtWorkspace::renderFileOpen
//

void OtWorkspace::renderFileOpen() {
	// handle file open dialog
	ImVec2 maxSize = ImGui::GetIO().DisplaySize;
	ImVec2 minSize = ImVec2(maxSize.x * 0.5, maxSize.y * 0.5);

	if (ImGuiFileDialog::Instance()->Display("workspace-open", ImGuiWindowFlags_NoCollapse, minSize, maxSize)) {
		// open selected file if required
		if (ImGuiFileDialog::Instance()->IsOk()) {
			auto dialog = ImGuiFileDialog::Instance();
			auto path = OtPathJoin(dialog->GetCurrentPath(), dialog->GetCurrentFileName());
			openFile(path, OtEditor::inTab);

			if (state != confirmErrorState) {
				state = editState;
			}

			OtPathChangeDirectory(OtPathGetParent(path));

		} else {
			state = editors.size() ? editState : splashState;
		}

		// close dialog
		ImGuiFileDialog::Instance()->Close();
	}
}


//
//	OtWorkspace::renderSaveAs
//

void OtWorkspace::renderSaveAs() {
	// handle saveas dialog
	ImVec2 maxSize = ImGui::GetIO().DisplaySize;
	ImVec2 minSize = ImVec2(maxSize.x * 0.5, maxSize.y * 0.5);

	if (ImGuiFileDialog::Instance()->Display("workspace-saveas", ImGuiWindowFlags_NoCollapse, minSize, maxSize)) {
		// open selected file if required
		if (ImGuiFileDialog::Instance()->IsOk()) {
			auto dialog = ImGuiFileDialog::Instance();
			auto path = OtPathJoin(dialog->GetCurrentPath(), dialog->GetCurrentFileName());
			path = OtPathReplaceExtension(path, activeEditor->getExtension());
			activeEditor->saveAsFile(path);
			state = editState;

			OtPathChangeDirectory(OtPathGetParent(path));

		} else {
			state = editors.size() ? editState : splashState;
		}

		// close dialog
		ImGuiFileDialog::Instance()->Close();
	}
}


//
//	OtWorkspace::renderConfirmClose
//

void OtWorkspace::renderConfirmClose() {
	ImGui::OpenPopup("Delete?");
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5, 0.5));

	if (ImGui::BeginPopupModal("Delete?", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Text("This file has changed!\nDo you really want to delete it?\n\n");
		ImGui::Separator();

		if (ImGui::Button("OK", ImVec2(120, 0))) {
			deleteEditor(activeEditor);
			state = editors.size() ? editState : splashState;
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
//	OtWorkspace::renderConfirmQuit
//

void OtWorkspace::renderConfirmQuit() {
	ImGui::OpenPopup("Quit Application?");
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5, 0.5));

	if (ImGui::BeginPopupModal("Quit Application?", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Text("You have unsaved files!\nDo you really want to quit?\n\n");
		ImGui::Separator();

		if (ImGui::Button("OK", ImVec2(120, 0))) {
			OtMessageBus::instance()->send("stop");
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
//	OtWorkspace::renderConfirmWarning
//

void OtWorkspace::renderConfirmWarning() {
	ImGui::OpenPopup("Warning");
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5, 0.5));

	if (ImGui::BeginPopupModal("Warning", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Text("%s\n", message.c_str());
		ImGui::Separator();

		if (ImGui::Button("OK", ImVec2(120, 0)) || ImGui::IsKeyPressed(ImGuiKey_Escape, false)) {
			state = editors.size() ? editState : splashState;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}


//
//	OtWorkspace::renderConfirmError
//

void OtWorkspace::renderConfirmError() {
	ImGui::OpenPopup("Error");
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5, 0.5));

	if (ImGui::BeginPopupModal("Error", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Text("%s\n", message.c_str());
		ImGui::Separator();

		if (ImGui::Button("OK", ImVec2(120, 0)) || ImGui::IsKeyPressed(ImGuiKey_Escape, false)) {
			state = editors.size() ? editState : splashState;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}


//
//	OtWorkspace::renderSubProcess
//

void OtWorkspace::renderSubProcess() {
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
	std::string title = fmt::format("Runnning [{}]...", currentRunnable);
	ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "%s", title.c_str());
	ImGui::SameLine(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - 150.0f);

	if (subprocess.isRunning()) {
		if (ImGui::Button("Kill Process", ImVec2(150.0f, 0.0f))) {
			subprocess.kill(SIGINT);
		}

	} else {
		if (ImGui::Button("Close Console", ImVec2(150.0f, 0.0f))) {
			consoleFullScreen = false;
		}

		if (ImGui::IsKeyDown(ImGuiMod_Ctrl) && ImGui::IsKeyPressed(ImGuiKey_W, false)) {
			consoleFullScreen = false;
		}
	}

	// render the console
	console.render();

	ImGui::End();
	ImGui::PopStyleVar();
}


//
//	OtWorkspace::getExecutablePath
//

std::string OtWorkspace::getExecutablePath() {
	// figure out where we live
	char buffer[1024];
	size_t length = 1024;
	auto status = uv_exepath(buffer, &length);
	UV_CHECK_ERROR("uv_exepath", status);
	buffer[length] = 0;
	return buffer;
}


//
//	OtWorkspace::highlightError
//

void OtWorkspace::highlightError(OtException& exception) {
	// see if the module is a valid ObjectTalk file
	auto module = exception.getModule();

	if (OtPathIsRegularFile(module) && OtPathGetExtension(module) == ".ot") {
		// see of this file is already open in the IDE
		auto editor = findEditor(module);

		if (!editor) {
			// open the editor
			openFile(module, OtEditor::inTab);
			editor = findEditor(module);
		}

		// activate the editor
		activateEditor(editor);

		// ask editor to highlight error
		auto scriptEditor = std::dynamic_pointer_cast<OtObjectTalkEditor>(editor);
		OtAssert(scriptEditor);

		scriptEditor->highlightError(exception.getLineNumber(), exception.getShortErrorMessage());
	}
}


//
//	OtWorkspace::determinePanelHeights
//

void OtWorkspace::determinePanelHeights() {
	// get available space in window
	auto available = ImGui::GetContentRegionAvail();

	// determine editors height
	editorsMinHeight = available.y * 0.05f;
	editorsMaxHeight = available.y * 0.9f;

	if (editorsHeight < 0.0) {
		editorsHeight =  available.y * 0.75f;

	} else {
		editorsHeight = std::clamp(editorsHeight, editorsMinHeight, editorsMaxHeight);
	}
}
