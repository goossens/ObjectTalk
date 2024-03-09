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
	// editor state
	enum {
		inTab,
		inWindow
	};

	// constructors/destructor
	virtual inline ~OtEditor() {}

	// these methods must be overriden by subclasses
	virtual void newFile(const std::string& path) {}
	virtual void openFile(const std::string& path) {}
	virtual void saveFile() {}
	virtual void saveAsFile(const std::string& path) {}
	virtual std::string getFileExtension() { return ""; }
	virtual std::string getFilePath() { return ""; }

	// render the editor parts
	virtual inline void renderMenu() {}
	virtual inline void renderEditor() {}

	// set the editor state
	inline void setVisualState(int vs) { visualState = vs; }
	inline bool isRenderedInTab() { return visualState == inTab; }
	inline bool isRenderedInWindow() { return visualState == inWindow; }

	// get editor status
	virtual inline bool isDirty() { return false; }

	// handle exception during a "run"
	virtual inline void error(OtException e) {}

protected:
	// render a file menu
	void renderFileMenu();

	// properties
	int visualState = inTab;
};
