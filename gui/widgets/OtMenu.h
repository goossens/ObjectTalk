//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtWidget.h"


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
	OtObject setTitle(const std::string& t) { title = t; return shared(); }
	std::string getTitle() { return title; }

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
};
