//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <filesystem>
#include <memory>

#include "OtSingleton.h"

#include "OtFrameworkApp.h"
#include "OtLogo.h"

#include "OtConsole.h"
#include "OtEditor.h"
#include "OtSubProcess.h"


//
//	OtWorkspaceClass
//

class OtWorkspaceClass;
typedef std::shared_ptr<OtWorkspaceClass> OtWorkspace;

class OtWorkspaceClass : public OtFrameworkApp, public OtSingleton<OtWorkspaceClass> {
public:
	// run the workspace
	void run();

	// create a new file
	void newFile();
	void newScript();
	void newScene();

	// open a file
	void openFile();
	void openFile(const std::filesystem::path& filename);

	// save content of active file/editor
	void saveFile();
	void saveAsFile();

	// close active file/editor
	void closeFile();

	// run the current file
	void runFile();

	// delete a specified editor
	void deleteEditor(std::shared_ptr<OtEditor> editor);

	// find a named editor
	std::shared_ptr<OtEditor> findEditor(const std::filesystem::path& filename);

	// make a specified editor active
	void activateEditor(std::shared_ptr<OtEditor> editor);

	// create a new workspace object
	static OtWorkspace create();

private:
	// framework callbacks
	void onRender() override;
	void onTerminate() override;
	bool onCanQuit() override;

	// create a name for an untitled file
	std::string getUntitledName();

	// get executable path and default path
	std::filesystem::path getExecutablePath();
	std::filesystem::path getDefaultDirectory();

	// get current working directory
	std::filesystem::path getCWD();
	std::filesystem::path cwd;

	// render parts of workspace
	void renderSplashScreen();
	void renderEditors();
	void renderNewFileType();
	void renderFileOpen();
	void renderSaveAs();
	void renderConfirmClose();
	void renderConfirmQuit();
	void renderConfirmError();
	void renderSubProcess();

	// the logo for the splash screen
	std::shared_ptr<OtLogo> logo;

	// list of open editors
	std::vector<std::shared_ptr<OtEditor>> editors;

	// track editors
	std::shared_ptr<OtEditor> activeEditor;
	std::shared_ptr<OtEditor> activateEditorTab;

	// tab for "save as" dialog
	OtEditor saveAsEditor;

	// error message
	std::string errorMessage;

	// workspace state
	enum {
		splashState,
		editState,
		newFileState,
		openFileState,
		saveFileAsState,
		confirmCloseState,
		confirmQuitState,
		confirmErrorState
	} state = splashState;

	// stuff to run things and show the console
	OtSubProcess subprocess;
	std::filesystem::path currentRunnable;

	OtConsole console;
	bool consoleFullScreen = false;
	bool consoleVisible = false;
	float editorsHeight = -1.0f;
};
