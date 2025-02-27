//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "OtException.h"

#include "OtFrameworkApp.h"
#include "OtLogo.h"

#include "OtConsole.h"
#include "OtDebuggerUI.h"
#include "OtEditor.h"
#include "OtSubProcess.h"


//
//	OtWorkspace
//

class OtWorkspace : public OtFrameworkApp {
public:
	// constructor
	OtWorkspace();

	// handle message bus commands
	void onMessage(const std::string& message);

	// create a new file
	void newFile();
	void newFile(const std::string& extension);

	// open a file
	void openFile();
	void openFile(const std::string& filename, OtEditor::VisualState visualState);

	// save content of active file/editor
	void saveFile();
	void saveAsFile();

	// close active file/editor
	void closeFile();

	// run the current file
	void runFile();

	// move all editors to floating window or tabs
	void windifyAllEditors();
	void TabifyAllEditors();

	// delete a specified editor
	void deleteEditor(std::shared_ptr<OtEditor> editor);

	// find a named editor
	std::shared_ptr<OtEditor> findEditor(const std::string& filename);

	// make a specified editor active
	void activateEditor(std::shared_ptr<OtEditor> editor);

	// register an editor type
	template <typename T>
	void registerEditorType() {
		editorTypes[T::extension] = []() { return std::make_shared<T>(); };
	}

private:
	// framework callbacks
	void onSetup() override;
	void onRender() override;
	void onTerminate() override;
	bool onCanQuit() override;

	// create an editor by file extension
	std::shared_ptr<OtEditor> createEditor(const std::string& extension);

	// highlight error in editor after subprocess run
	void highlightError(OtException& exception);

	// render parts of workspace
	void renderSplashScreen();
	void renderEditors();
	void renderTabbedEditors();
	void renderWindowedEditors();
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

	// list of editor types
	std::unordered_map<std::string, std::function<std::shared_ptr<OtEditor>()>> editorTypes;

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
	enum class State {
		splash,
		edit,
		newFile,
		openFile,
		saveFileAs,
		confirmClose,
		confirmQuit,
		confirmWarning,
		confirmError
	};

	State state = State::splash;

	// stuff to run things and show the debugger/console
	OtSubProcess subprocess;
	std::string currentRunnable;

	OtConsole console;
	OtDebuggerUI debugger;
	bool consoleAsPanel = false;
	bool showDebugger = false;

	// workspace layout parameters
	void determinePanelHeights();
	void determinePanelWidths();
	float contentsHeight = -1.0f;
	float contentsMinHeight;
	float contentsMaxHeight;
};
