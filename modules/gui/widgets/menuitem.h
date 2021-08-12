//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "widget.h"


//
//	OtMenuItemClass
//

class OtMenuItemClass;
typedef std::shared_ptr<OtMenuItemClass> OtMenuItem;

class OtMenuItemClass : public OtWidgetClass {
public:
	// initialize menu item
	void init(const std::string& t, const std::string& s) { title = t; shortcut = s; }

	// adjust menu item properties
	void setTitle(const std::string& t) { title = t; }
	std::string getTitle() { return title; }

	void setShortcut(const std::string& s) { shortcut = s; }
	std::string getShortcut() { return shortcut; }

	void select() { selected = true; }
	void unselect() { selected = false; }
	bool isSelected() { return selected; }

	void enable() { enabled = true; }
	void disable() { enabled = false; }
	bool isEnabled() { return enabled; }

	// render content
	void render();

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtMenuItem create();

private:
	// menu item properties
	std::string title = "Item";
	std::string shortcut = "";
	bool selected = false;
	bool enabled = true;
};
