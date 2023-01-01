//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtModule.h"
#include "OtOS.h"

#include "OtApp.h"
#include "OtFramework.h"
#include "OtGui.h"

#include "OtConsole.h"
#include "OtObjectTalkEditor.h"
#include "OtScriptRunner.h"
#include "OtWorkspace.h"


//
//	OtScriptRunnerClass::OtScriptRunnerClass
//

OtScriptRunnerClass::OtScriptRunnerClass() {
	// intercept calls to start and stop servers (we're in charge now)
	OtOSClass::instance()->registerServer(
		// called when a server is started
		[this]() {
			// this gets called when a script runs os.runServer()
			// we simply wait until the server quits
			// we are already running a main loop so no need to run it again
			runningServer = true;
			std::unique_lock<std::mutex> lock(mutex);
			cv.wait(lock);

			// see if a GUI exception happened; if it did, we throw it here
			if (hasException) {
				runningServer = false;
				hasException = false;
				throw exception;
			}
		},

		// called when we have an exception during the server run
		[this](OtException e) {
			// something happened in the script; let's handle exception and stop script
			runningServer = false;
			hasException = true;
			exception = e;
			cv.notify_all();
		},

		// called during normal server termination
		[this]() {
			// this gets called when a script runs os.stopGUI()
			runningServer = false;
			cv.notify_all();
		});

	// intercept calls to start and stop GUI app (we're in charge now)
	OtOSClass::instance()->registerGUI(
		// called when a GUI is started
		[this]() {
			// this gets called when a script runs os.runServer()
			// we simply hook the app up to the framework and wait until it quits
			// we are already running a main loop so no need to run it again
			runningGUI = true;
			auto app = OtGuiClass::getRegisteredApp();

			// sanity check
			if (!app) {
				OtExcept("You can't run a GUI without an instance of the [App] class");
			}

			OtFrameworkClass::instance()->addCustomer(app);
			std::unique_lock<std::mutex> lock(mutex);
			cv.wait(lock);
			OtFrameworkClass::instance()->removeCustomer(app);

			// see if a GUI exception happened; if it did, we throw it here
			if (hasException) {
				runningGUI = false;
				hasException = false;
				throw exception;
			}
		},

		// called when we have an exception during the GUI run
		[this](OtException e) {
			// something happened in the script; let's handle exception and stop script
			runningGUI = false;
			hasException = true;
			exception = e;
			cv.notify_all();
		},

		// called during normal GUI termination
		[this]() {
			// this gets called when a script runs os.stopGUI()
			runningGUI = false;
			cv.notify_all();
		});
}


//
//	OtScriptRunnerClass::run
//

void OtScriptRunnerClass::run(const std::string& filename) {
	// clear the console
	OtConsoleClass::instance()->clear();

	// run the script in a new thread
	thread = std::thread([this, filename]() {
		// the module gets destroyed in the main GUI thread (to avoid race conditions)
		runningScript = true;
		OtModule module = OtModuleClass::create();

		try {
			// load and run module
			module->load(filename);
			events.pushFinishedNormallyEvent(module);

		} catch (const OtException& e) {
			events.pushFinishedWithExceptionEvent(module, e);
		}

		runningScript = false;
	});
}


//
//	OtScriptRunnerClass::update
//

void OtScriptRunnerClass::update() {
	// process events to ensure things happen in the right thread
	while (events.size()) {
		Event event = events.pop();
		OtWorkspace workspace = OtWorkspaceClass::instance();
		OtConsole console = OtConsoleClass::instance();
		OtEditor editor;

		switch (event.type) {
			case Event::finishedNormally:
				event.module->unsetAll();
				thread.join();
				break;

			case Event::finishedWithException:
				// write message to console
				console->writeError(event.exception.getLongErrorMessage());
				console->writeError("\n");

				// open erroneous module in editor
				editor = workspace->findEditor(event.exception.getModule());

				if (!editor) {
					editor = workspace->openFile(event.exception.getModule());
				}

				// highlight error in code
				editor->cast<OtObjectTalkEditorClass>()->highlightError(
					event.exception.getLineNumber(),
					event.exception.getShortErrorMessage());

				workspace->activateEditor(editor);
				event.module->unsetAll();
				thread.join();
				break;
		}
	}
}
