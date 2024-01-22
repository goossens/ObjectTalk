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
#include "OtScriptAsset.h"


//
//	OtObjectTalkEditor
//

class OtObjectTalkEditor : public OtEditor {
public:
	// constructor
	OtObjectTalkEditor(const std::string& path) : OtEditor(path) {
		initialize();
	}

	// initialize the editor
	void initialize();

	// get file extension
	std::string getFileExtension() override { return ".ot"; }

	// load/save content
	void load() override;
	void save() override;

	// render the parts
	void renderMenu() override;
	void renderEditor() override;

	// is the editor's content "dirty" (unsaved);
	bool isDirty() override;

	// update error markers
	void highlightError(size_t line, const std::string& error);
	void clearError();

private:
	// the graph being edited
	OtAsset<OtScriptAsset> asset;

	// visual text editor
	TextEditor editor;

	// properties
	int version = 0;
	int scrollToLine = 0;
};
