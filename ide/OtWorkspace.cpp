//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <filesystem>

#include "ImGuiFileDialog.h"

#include "OtCerr.h"
#include "OtCout.h"
#include "OtLibuv.h"
#include "OtModule.h"
#include "OtOS.h"

#include "OtFramework.h"

#include "OtWorkspace.h"


//
//	OtWorkspaceClass::run
//

void OtWorkspaceClass::run() {
	// add ourselves to the GUI framework as a customer
	OtFrameworkClass::instance()->addCustomer(this);

	// add a console window
	console = OtConsoleClass::create();

	// intercept cout
	OtCoutClass::instance()->setOutputFunction([this](const std::string& text) {
		events.writeToOutEvent(text);
		showConsole = true;
	});

	// intercept cerr
	OtCerrClass::instance()->setOutputFunction([this](const std::string& text) {
		events.writeToErrEvent(text);
		showConsole = true;
	});

	// intercept calls to start and stop the framework (we're in charge now)
	OtOSClass::instance()->registerGUI(
		std::bind(&OtWorkspaceClass::onRunApp, this),
		std::bind(&OtWorkspaceClass::onErrorApp, this, std::placeholders::_1),
		std::bind(&OtWorkspaceClass::onStopApp, this));

	// run the GUI framework
	OtFrameworkClass::instance()->run();
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
//	OtWorkspaceClass::highlightError
//

void OtWorkspaceClass::highlightError(const std::string module, size_t line, std::string error) {
	OtEditor editor = findEditor(module);

	if (!editor) {
		openFile(module);
		editor = findEditor(module);
	}

	editor->highlightError(line, error);
}


//
//	OtWorkspaceClass::runFile
//

void OtWorkspaceClass::runFile(const std::string& filename) {
	// start a new thread
	started = true;
	running = true;

	console->write("\n");
	console->scrollToBottom();

	thread = std::thread([this, filename]() {
		OtModule module;

		// hide editors and console
		showEditors = false;
		showConsole = false;

		try {
			// load and run module
			module = OtModuleClass::create(filename);
			events.pushFinishedWithoutErrorsEvent();

		} catch (const OtException& e) {
			events.pushFinishedWithExceptionEvent(e);
		}

		// if this module had a GUI, do the cleanup
		if (customer) {
			customer->onTerminate();
		}

		// little house keeping
		running = false;
		showEditors = true;
		showConsole = true;
	});
};


//
//	OtWorkspaceClass::onRunApp
//

void OtWorkspaceClass::onRunApp() {
	// this gets called when a script runs os.runGUI()
	// we simply wait until the app quits
	// as the framework is already running the main loop
	std::unique_lock<std::mutex> lock(mutex);
	cv.wait(lock);
}


//
//	OtWorkspaceClass::onErrorApp
//

void OtWorkspaceClass::onErrorApp(OtException e) {
	// something happend in the script; let's handle exception and stop script
	events.pushFinishedWithExceptionEvent(e);
	cv.notify_all();
}


//
//	OtWorkspaceClass::stopApp
//

void OtWorkspaceClass::onStopApp() {
	// this gets called when a script runs os.stopGUI()
	cv.notify_all();
}


//
//	OtWorkspaceClass::onAddCustomer
//

void OtWorkspaceClass::onAddCustomer(OtCustomer* c) {
	customer = c;
}


//
//	OtWorkspaceClass::onRemoveCustomer
//

void OtWorkspaceClass::onRemoveCustomer(OtCustomer* c) {
	customer = nullptr;
}


//
//	OtWorkspaceClass::onRender
//

void OtWorkspaceClass::onRender() {
	// process all events
	procesEvents();

	// render console (if required)
	if (showConsole) {
		console->render();
	}

	// render all editors (if required)
	if (showEditors) {
		// make clone of editor list since renderers might change it
		std::vector<OtEditor> clone = editors;

		for (auto& editor : clone) {
			editor->render();
		}
	}

	// handle file open dialog
	OtFramework framework = OtFrameworkClass::instance();
	int width = framework->getWidth();
	int height = framework->getHeight();
	ImVec2 maxSize = ImVec2(width, height);
	ImVec2 minSize = ImVec2(width * 0.5, height * 0.5);

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

	// handle "quit" confirmation (when user quits without saving)
	if (confirmQuit) {
		ImGui::OpenPopup("Quit Application?");
		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5, 0.5));

		if (ImGui::BeginPopupModal("Quit Application?", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::Text("You have unsaved files!\nDo you really want to Quit?\n\n");
			ImGui::Separator();

			if (ImGui::Button("OK", ImVec2(120, 0))) {
				framework->stop();
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
	// is a script running at the moment?
	if (running) {
		// just stop the script and keep the IDE going
		onStopApp();
		return false;

	} else {
		// we can't quit if we still have a "dirty" editor
		bool canQuit = true;

		for (auto& editor : editors) {
			if (editor->isDirty()) {
				canQuit = false;
				confirmQuit = true;
			}
		}

		return canQuit;
	}
}


//
//	OtWorkspaceClass::procesEvents
//

void OtWorkspaceClass::procesEvents() {
	while (events.size()) {
		OtIdeEvent event = events.pop();

		switch (event.type) {
			case OtIdeEvent::writeToOut:
				console->write(event.longMessage);
				break;

			case OtIdeEvent::writeToErr:
				console->writeError(event.longMessage);
				break;

			case OtIdeEvent::finishedWithException:
				OtCerrClass::instance()->write(event.longMessage);
				OtCerrClass::instance()->write("\n");
				highlightError(event.module, event.lineNumber, event.shortMessage);
				thread.join();
				started = false;
				break;

			case OtIdeEvent::finishedWithoutErrors:
				if (started) {
					thread.join();
					started = false;
				}

				break;
		}
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
//	OtWorkspaceClass::getMeta
//

OtType OtWorkspaceClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtWorkspaceClass>(
			"Workspace",
			OtIdeClass::getMeta(),
			[]() {
				return (OtObject) OtWorkspaceClass::instance();
			});
	}

	return type;
}


//
//	OtWorkspaceClass::create
//

OtWorkspace OtWorkspaceClass::create() {
	return OtWorkspaceClass::instance();
}
