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

#include "OtEditor.h"


//
//	OtObjectTalkEditorClass
//

class OtObjectTalkEditorClass;
typedef std::shared_ptr<OtObjectTalkEditorClass> OtObjectTalkEditor;

class OtObjectTalkEditorClass : public OtEditorClass {
public:
	// constructor
	OtObjectTalkEditorClass();

	// render the editor
	void render(float width, float height) override;

	// is the editor's content "dirty" (unsaved);
	bool isDirty() override;

	// update error markers
	void highlightError(size_t line, const std::string& error);
	void clearError();

	// create a new object
	static OtObjectTalkEditor create();
	static OtObjectTalkEditor create(const std::string& filename);

private:
	// file actions
	void loadFile(const std::string& filename);
	void newFile();
	void openFile();
	void saveFile();
	void saveAsFile();
	void closeFile();
	void compileFile();
	void runFile();

	// visual text editor
	TextEditor editor;

	// properties
	int version = 0;
	bool confirmClose = false;
	int scrollToLine = 0;
};
