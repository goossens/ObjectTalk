//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <condition_variable>
#include <mutex>
#include <thread>

#include "OtModule.h"
#include "OtSingleton.h"

#include "OtCustomer.h"

#include "OtConsole.h"
#include "OtIde.h"
#include "OtIdeEvents.h"
#include "OtEditor.h"


//
//	OtWorkspaceClass
//

class OtWorkspaceClass;
typedef std::shared_ptr<OtWorkspaceClass> OtWorkspace;

class OtWorkspaceClass : public OtIdeClass, public OtCustomer, public OtObjectSingleton<OtWorkspaceClass> {
public:
	// run the IDE
	void run();

	// create a new file
	void newFile();

	// open a file
	void openFile();
	void openFile(const std::string& filename);

	// close editor
	void closeEditor(OtEditor editor);

	// find a named editor
	OtEditor findEditor(const std::string& filename);

	// see if we can run a file
	bool canRunFile() { return !started && !running; }

	// run a file
	void runFile(const std::string& filename);
	void onRunGUI();
	void onErrorGUI(OtException e);
	void onStopGUI();

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtWorkspace create();

private:
	// track other framework customers
	void onAddCustomer(OtCustomer* customer) override;
	void onRemoveCustomer(OtCustomer* customer) override;

	// update state and process events
	void onUpdate() override;

	// render all windows
	void onRender() override;

	// see if we can quit app
	bool onCanQuit() override;

	// get default directory
	std::string getDefaultDirectory();

	// get current working directory
	std::string getCWD();
	std::string cwd;

	// list of open editors
	std::vector<OtEditor> editors;

	// our console
	OtConsole console;

	// visibility of editors and console
	bool showEditors = true;
	bool showConsole = true;

	// quit confirmation
	bool confirmQuit = false;

	// here is where we run code
	std::thread thread;
	bool started = false;
	bool running = false;

	// tracking state of thread (when a GUI app is running)
	std::mutex mutex;
	std::condition_variable cv;
	OtCustomer* customer;

	// the module that we are running
	OtModule module;

	// for cross thread comminication
	OtIdeEventQueue events;
};
