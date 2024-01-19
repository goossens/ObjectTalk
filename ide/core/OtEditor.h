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

#include "OtException.h"

#include "OtPathTools.h"


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
	void render();
	virtual inline void startRender() {}
	virtual inline void renderMenu() {}
	virtual inline void renderEditor() {}
	virtual inline void endRender() {}

	// get the properties
	virtual inline std::string getFileExtension() { return ""; }
	inline std::string getFilePath() { return path; }
	inline std::string getShortName() { return OtPathGetFilename(path); }

	// get the properties
	void setFilePath(const std::string& path);

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
	std::string path;
};
