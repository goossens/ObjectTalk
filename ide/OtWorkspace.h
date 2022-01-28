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

	// highlight error in editor
	void highlightError(const std::string module, size_t line, const std::string error);

	// see if we can run a file
	bool canRunFile() { return !started && !running; }

	// run a file
	void runFile(const std::string& filename);
	void onRunApp();
	void onErrorApp(OtException e);
	void onStopApp();

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtWorkspace create();

private:
	// track other framework customers
	void onAddCustomer(OtCustomer* customer);
	void onRemoveCustomer(OtCustomer* customer);

	// render all windows
	void onRender();

	// see if we can quit app
	bool onCanQuit();

	// process events
	void procesEvents();

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

	// for cross thread comminication
	OtIdeEventQueue events;
};
