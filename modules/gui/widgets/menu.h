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
//	OtMenuClass
//

class OtMenuClass;
typedef std::shared_ptr<OtMenuClass> OtMenu;

class OtMenuClass : public OtWidgetClass {
public:
	// initialize menu
	void init(const std::string& t) { title = t; }

	// adjust menu properties
	void setTitle(const std::string& t) { title = t; }
	std::string getTitle() { return title; }

	void enable() { enabled = true; }
	void disable() { enabled = false; }
	bool isEnabled() { return enabled; }

	// ensure specified component is allowed as a child
	void validateChild(OtComponent child);

	// render content
	void render();

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtMenu create();

private:
	// menu properties
	std::string title = "Menu";
	bool enabled = true;
};
