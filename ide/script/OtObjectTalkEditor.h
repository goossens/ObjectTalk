//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>

#include "TextEditor.h"

#include "OtEditor.h"


//
//	OtObjectTalkEditor
//

class OtObjectTalkEditor : public OtEditor {
public:
	// constructor
	OtObjectTalkEditor();

	// get editor status
	inline bool isRunnable() override { return true; }
	inline bool isDirty() override { return editor.GetUndoIndex() != version; }
	inline std::string getExtension() override { return ".ot"; }

	// render the parts
	void renderMenu() override;
	void renderEditor() override;

	// update error markers
	void highlightError(size_t line, const std::string& error);
	void clearError();

private:
	// file access functions
	void clear() override;
	void load() override;
	void save() override;

	// visual text editor
	TextEditor editor;
	bool focusOnEditor = true;

	// find and replace support
	bool findReplaceVisible = false;
	bool focusOnFind = false;
	std::string findText;
	std::string replaceText;
	bool caseSensitiveFind = false;
	bool wholeWordFind = false;

	void openFindReplace();
	void find();
	void findAll();
	void replace();
	void replaceAll();

	// properties
	int version = 0;
	int scrollToLine = 1;
};
