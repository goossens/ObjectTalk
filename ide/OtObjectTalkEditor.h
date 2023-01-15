//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
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

	// get file extension
	std::string getFileExtension() override { return ".ot"; }

	// load save content
	void load() override;
	void save() override;

	// render the editor
	void render() override;

	// is the editor's content "dirty" (unsaved);
	bool isDirty() override;

	// update error markers
	void highlightError(size_t line, const std::string& error);
	void clearError();

	// create a new object
	static OtObjectTalkEditor create(const std::string& filename);

private:
	// render the parts
	void renderMenu();
	void renderEditor();

	// file actions
	void compile();
	void run();

	// visual text editor
	TextEditor editor;

	// properties
	int version = 0;
	int scrollToLine = 0;
	float editorHeight = -1.0;
};
