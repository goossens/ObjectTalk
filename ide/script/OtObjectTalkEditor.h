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
	OtObjectTalkEditor();

	// file handling functions
	void newFile(const std::string& path) override;
	void openFile(const std::string& path) override;
	void saveFile() override;
	void saveAsFile(const std::string& path) override;
	inline std::string getFileExtension() override { return ".ot"; }
	inline std::string getFilePath() override { return asset->getPath(); }

	// get editor status
	inline bool isReady() override { return asset.isReady(); }
	bool isDirty() override;

	// render the parts
	void renderMenu() override;
	void renderEditor() override;

	// update error markers
	void highlightError(size_t line, const std::string& error);
	void clearError();

private:
	// the script being edited
	OtAsset<OtScriptAsset> asset;

	// visual text editor
	TextEditor editor;

	// properties
	int version = 0;
	int scrollToLine = 0;
};
