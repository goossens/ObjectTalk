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
//	OtScreenClass
//

class OtScreenClass;
typedef std::shared_ptr<OtScreenClass> OtScreen;

class OtScreenClass : public OtWidgetClass {
public:
	// ensure specified component is allowed as a child
	void validateChild(OtComponent child);

	// get menubar height
	size_t getMenubarHeight();

	// mouse and keyboard events
	void onMouseButton(int button, int action, int mods, double xpos, double ypos);
	void onMouseMove(double xpos, double ypos);
	void onMouseDrag(int button, int mods, double xpos, double ypos);
	void onScrollWheel(double dx, double dy);
	void onKey(int key, int mods);
	void onChar(unsigned int codepoint);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtScreen create();
};
