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
//	OtWindowClass
//

class OtWindowClass;
typedef std::shared_ptr<OtWindowClass> OtWindow;

class OtWindowClass : public OtWidgetClass {
public:
	// initialize window
	void init(const std::string& t, int w, int h, int f) { title = t; width = w; height = h; flags = f; }

	// adjust window properties
	void setTitle(const std::string& t) { title = t; }
	std::string getTitle() { return title; }

	void setFlags(int f) { flags = f; }
	int getFlags() { return flags; }

	// ensure specified component is allowed as a child
	void validateChild(OtComponent child);

	// render content
	void render();

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtWindow create();

private:
	std::string title;
	int width = 600;
	int height = 300;
	int flags = 0;
};
