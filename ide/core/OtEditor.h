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
#include "OtPathFollower.h"


//
//	OtEditorClass
//

class OtEditor {
public:
	// editor visual state
	enum class VisualState {
		inTab,
		inWindow
	};

	// constructors/destructor
	virtual inline ~OtEditor() {}

	// file I/O operations
	void newFile(const std::string& path);
	void openFile(const std::string& path);
	void saveFile();
	void saveAsFile(const std::string& path);
	std::string getPath() { return path; }

	// render the editor parts
	virtual inline void renderMenu(bool canRun) {}
	virtual inline void renderEditor() {}

	// access the editor state
	inline void setVisualState(VisualState vs) { visualState = vs; }
	inline bool isRenderedInTab() { return visualState == VisualState::inTab; }
	inline bool isRenderedInWindow() { return visualState == VisualState::inWindow; }

	virtual inline bool isRunnable() { return false; }
	virtual inline bool isDirty() { return false; }
	virtual inline std::string getExtension() { return ""; }

	// handle exception during a "run"
	virtual inline void error(OtException e) {}

protected:
	// these methods must be overriden by subclasses
	virtual void clear() {}
	virtual void load() {}
	virtual void save() {}

	// follow file changes and update (if possible)
	void follow();
	void unfollow();
	bool ignoreNextFileUpdate = false;

	// render a file menu
	void renderFileMenu(bool canRun);

	// properties
	std::string path;
	OtPathFollower follower;
	VisualState visualState = VisualState::inTab;
};
