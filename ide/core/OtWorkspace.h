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

#include "OtSingleton.h"

#include "OtFrameworkCustomer.h"
#include "OtTexture.h"

#include "OtEditor.h"


//
//	OtWorkspaceClass
//

class OtWorkspaceClass;
typedef std::shared_ptr<OtWorkspaceClass> OtWorkspace;

class OtWorkspaceClass : public OtFrameworkCustomer, public OtSingleton<OtWorkspaceClass> {
public:
	// run the IDE
	void run();

	// create a new file
	void newFile();
	void newScript();
	void newScene();

	// open a file
	void openFile();
	void openFile(const std::filesystem::path& filename);

	// save file content
	void saveFile();
	void saveAsFile();

	// close file
	void closeFile();

	// delete an editor
	void deleteEditor(OtEditor editor);

	// find a named editor
	OtEditor findEditor(const std::filesystem::path& filename);

	// make a specified editor tab active
	void activateEditor(OtEditor editor);

	// create a new workspace object
	static OtWorkspace create();

private:
	// create a name for an untitled file
	std::string getUntitledName();

	// update state and process events
	void onUpdate() override;

	// render all windows
	void onRender() override;

	// see if we can quit app
	bool onCanQuit() override;

	// get default directory
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

	// splash screen logo
	OtTexture logo;

	// list of open editors
	std::vector<OtEditor> editors;

	// track editors
	OtEditor activeEditor;
	OtEditor activateEditorTab;

	// tab for save as dialog
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
};
