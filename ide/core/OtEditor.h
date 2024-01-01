//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
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
	virtual inline ~OtEditor() {}

	// load/save the document content
	virtual inline void load() {}
	virtual inline void save() {}

	// render the editor
	void render(bool active);
	virtual inline void startRender() {}
	virtual inline void renderMenu() {}
	virtual inline void renderEditor(bool active) {}
	virtual inline void endRender() {}

	// get the properties
	virtual inline std::string getFileExtension() { return ""; }
	inline std::filesystem::path getFilePath() { return path; }
	inline std::string getFileName() { return path.string(); }
	inline std::string getShortName() { return path.filename().string(); }

	// get the properties
	void setFilePath(const std::filesystem::path& path);

	// get editor status
	virtual inline bool isDirty() { return false; }
	bool fileExists();

	// handle exception during a "run"
	virtual inline void error(OtException e) {}

protected:
	// render a file menu
	void renderFileMenu();

	// render common items on view menu
	void renderCommonViewMenuItems();

	// properties
	std::filesystem::path path;
};
