//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "TextEditor.h"

#include "OtIde.h"


//
//	OtEditorClass
//

class OtEditorClass;
typedef std::shared_ptr<OtEditorClass> OtEditor;

class OtEditorClass : public OtIdeClass {
public:
	// render the editor
	void render();

	// file actions
	void loadFile();
	void newFile();
	void openFile();
	void saveFile();
	void saveAsFile();
	void closeFile();
	void compileFile();
	void runFile();

	// get the name of the file being edited
	std::string getFileName() { return filename; }

	// is the editor's content "dirty" (unsaved);
	bool isDirty();

	// update error markers
	void highlightError(size_t line, const std::string error);
	void clearError();

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtEditor create();
	static OtEditor create(const std::string& filename);

private:
	// the file we are editing
	std::string filename;

	// visual text editor
	TextEditor editor;

	// window ID
	std::string id;

	// dirty state
	int version = 0;
	bool confirmClose = false;
};