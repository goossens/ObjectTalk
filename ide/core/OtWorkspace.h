//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <memory>
#include <string>

#include "OtException.h"

#include "OtFrameworkApp.h"
#include "OtLogo.h"

#include "OtConsole.h"
#include "OtEditor.h"
#include "OtSubProcess.h"


//
//	OtWorkspace
//

class OtWorkspace : public OtFrameworkApp {
public:
	// handle message bus commands
	void onMessage(const std::string& message);

	// create a new file
	void newFile();
	void newScript();
	void newScene();
	void newNodes();

	// open a file
	void openFile();
	void openFile(const std::string& filename, int visualState);

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
	std::shared_ptr<OtEditor> findEditor(const std::string& filename);

	// make a specified editor active
	void activateEditor(std::shared_ptr<OtEditor> editor);

private:
	// framework callbacks
	void onSetup() override;
	void onRender() override;
	void onTerminate() override;
	bool onCanQuit() override;

	// create a name for an untitled file
	std::string getUntitledName(const char* ext);

	// highlight error in editor after subprocess run
	void highlightError(OtException& exception);

	// render parts of workspace
	void renderSplashScreen();
	void renderEditors();
	void renderNewFileType();
	void renderFileOpen();
	void renderSaveAs();
	void renderConfirmClose();
	void renderConfirmQuit();
	void renderConfirmWarning();
	void renderConfirmError();
	void renderSubProcess();

	// the logo for the splash screen
	std::shared_ptr<OtLogo> logo;

	// list of open editors
	std::vector<std::shared_ptr<OtEditor>> editors;

	// track editors
	std::shared_ptr<OtEditor> activeEditor;
	std::shared_ptr<OtEditor> editorToActivate;

	// tab for "save as" dialog
	OtEditor saveAsEditor;

	// message
	std::string message;

	// workspace state
	enum {
		splashState,
		editState,
		newFileState,
		openFileState,
		saveFileAsState,
		confirmCloseState,
		confirmQuitState,
		confirmWarningState,
		confirmErrorState
	} state = splashState;

	// stuff to run things and show the console
	OtSubProcess subprocess;
	std::string currentRunnable;

	OtConsole console;
	bool consoleFullScreen = false;
	bool consoleAsPanel = false;
	float editorsHeight = -1.0f;
	float editorsMinHeight;
	float editorsMaxHeight;
	void determinePanelHeights();
};
