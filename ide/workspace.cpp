//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <filesystem>

#include "ot/libuv.h"

#include "application.h"

#include "workspace.h"


//
//	OtWorkspaceClass::init
//

void OtWorkspaceClass::init() {
	// register ourselves with the app so we get called every frame
	// and have is say in weither the app quits
	OtApplication application = OtApplicationClass::instance();
	application->atrender(std::bind(&OtWorkspaceClass::render, this));
	application->atclose(std::bind(&OtWorkspaceClass::close, this));
}


//
//	OtWorkspaceClass::newFile
//

void OtWorkspaceClass::newFile() {
	// don't create new files when we're in the process of opening one
	if (!ImGuiFileDialog::Instance()->IsOpened()) {
		editors.push_back(OtEditorClass::create());
	}
}


//
//	OtWorkspaceClass::openFile
//

void OtWorkspaceClass::openFile() {
	// open file select dialog (if one isn't open already)
	if (!ImGuiFileDialog::Instance()->IsOpened()) {
		ImGuiFileDialog::Instance()->OpenModal(
			"workspace-open",
			"Select File to Open...",
			".ot",
			getCWD(),
			1,
			nullptr,
			ImGuiFileDialogFlags_DontShowHiddenFiles |
				ImGuiFileDialogFlags_ReadOnlyFileNameField);
	}
}


//
//	OtWorkspaceClass::openFile
//

void OtWorkspaceClass::openFile(const std::string& filename) {
	// save the file's path as the current working directory
	cwd = std::filesystem::path(filename).parent_path().string();

	// open a new editor
	editors.push_back(OtEditorClass::create(filename));
}


//
//	OtWorkspaceClass::closeEditor
//

void OtWorkspaceClass::closeEditor(OtEditor editor) {
	// use remove_if when we move the C++20
	for (auto it = editors.begin(); it != editors.end();) {
		if ((*it)->equal(editor)) {
			it = editors.erase(it);

		} else {
			it++;
		}
	}
}


//
//	OtWorkspaceClass::run
//

void OtWorkspaceClass::run() {
	// simply run the app framework (we get the callbacks)
	OtApplicationClass::instance()->run("ObjectTalk IDE");
}


//
//	OtWorkspaceClass::render
//

void OtWorkspaceClass::render() {
	// make clone of editor list since renderers might change it
	std::vector<OtEditor> clone = editors;

	// refresh all editors
	for (auto& editor : clone) {
		editor->render();
	}

	// handle file open dialog
	OtApplication application = OtApplicationClass::instance();
	int width = application->getWidth();
	int height = application->getHeight();
	ImVec2 maxSize = ImVec2(width, height);
	ImVec2 minSize = ImVec2(width * 0.6, height * 0.6);

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
			if (ImGui::IsKeyPressed('N')) {
				newFile();

			} else if (ImGui::IsKeyPressed('O')) {
				openFile();
			}
		}
	}

	// handle "quit" confirmation (when user quits application without saving)
	if (confirmQuit) {
		ImGui::OpenPopup("Quit Application?");
		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5, 0.5));

		if (ImGui::BeginPopupModal("Quit Application?", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::Text("You have unsaved files!\nDo you really want to Quit?\n\n");
			ImGui::Separator();

			if (ImGui::Button("OK", ImVec2(120, 0))) {
				application->quit();
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
//	OtWorkspaceClass::close
//

bool OtWorkspaceClass::close() {
	// we can't close the app if we still have a "dirty" editor
	bool canClose = true;

	for (auto& editor : editors) {
		if (editor->isDirty()) {
			canClose = false;
			confirmQuit = true;
		}
	}

	return canClose;
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
//	OtWorkspaceClass::getMeta
//

OtType OtWorkspaceClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtWorkspaceClass>(
			"Workspace",
			OtIdeClass::getMeta(),
			[]() { return (OtObject) OtWorkspaceClass::instance(); });
	}

	return type;
}


//
//	OtWorkspaceClass::create
//

OtWorkspace OtWorkspaceClass::create() {
	return OtWorkspaceClass::instance();
}
