//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtWidget.h"


//
//	OtMenuItemClass
//

class OtMenuItemClass;
using OtMenuItem = OtObjectPointer<OtMenuItemClass>;

class OtMenuItemClass : public OtWidgetClass {
public:
	// initialize menu item
	void init(size_t count, OtObject* parameters);

	// adjust menu item properties
	OtObject setTitle(const std::string& t) { title = t; return OtObject(this); }
	std::string getTitle() { return title; }

	OtObject setShortcut(const std::string& s);
	std::string getShortcut() { return shortcut; }

	OtObject setCallback(OtObject callback);
	OtObject getCallback() { return callback; }

	void select() { selected = true; }
	void unselect() { selected = false; }
	bool isSelected() { return selected; }

	void enable() { enabled = true; }
	void disable() { enabled = false; }
	bool isEnabled() { return enabled; }

	// render content
	void render() override;

	// get type definition
	static OtType getMeta();

private:
	// menu item properties
	std::string title = "Item";
	std::string shortcut = "";
	bool selected = false;
	bool enabled = true;
	OtObject callback;
};
