//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

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

	// open a file
	void openFile();
	OtEditor openFile(const std::string& filename);

	// close editor
	void closeEditor(OtEditor editor);

	// find a named editor
	OtEditor findEditor(const std::string& filename);

	// make a specified editor tab active
	void activateEditor(OtEditor editor);

	// track running scripts
	void scriptStarted() { runningScripts++; }
	void scriptStopped() { runningScripts--; }

	// create a new object
	static OtWorkspace create();

private:
	// update state and process events
	void onUpdate() override;

	// render all windows
	void onRender() override;

	// see if we can quit app
	bool onCanQuit() override;
	size_t runningScripts = 0;

	// get default directory
	std::string getDefaultDirectory();

	// get current working directory
	std::string getCWD();
	std::string cwd;

	// render parts of wokspace
	void renderSplashScreen();
	void renderEditors();
	void renderFileOpen();
	void renderQuitConfirmation();

	// splash screen logo
	OtTexture logo;

	// list of open editors
	std::vector<OtEditor> editors;

	// tab to activate durong next frame
	OtEditor activateEditorTab;

	// quit confirmation
	bool confirmQuit = false;
};
