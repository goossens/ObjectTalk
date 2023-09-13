//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <filesystem>
#include <string>

#include "OtException.h"


//
//	OtEditorClass
//

class OtEditor {
public:
	// destructor
	virtual ~OtEditor() {}

	// load/save the document content
	virtual void load() {}
	virtual void save() {}

	// render the editor
	void render(bool active);
	virtual void renderMenu() {}
	virtual void renderEditor(bool active) {}

	// get the properties
	virtual std::string getFileExtension() { return ""; }
	std::filesystem::path getFilePath() { return path; }
	std::string getFileName() { return path.string(); }
	std::string getShortName() { return path.filename().string(); }

	// get the properties
	void setFilePath(const std::filesystem::path& path);

	// get editor status
	virtual bool isDirty() { return false; }
	bool fileExists();

	// handle exception during a "run"
	virtual void error(OtException e) {}

protected:
	// render a file menu
	void renderFileMenu();

	// render common items on view menu
	void renderCommonViewMenuItems();

	// properties
	std::filesystem::path path;
};
