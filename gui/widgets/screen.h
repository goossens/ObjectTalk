//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
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
	void onMouseButton(int button, int action, int mods, float xpos, float ypos);
	void onMouseMove(float xpos, float ypos);
	void onMouseDrag(int button, int mods, float xpos, float ypos);
	void onScrollWheel(float dx, float dy);
	void onKey(int key, int mods);
	void onChar(unsigned int codepoint);
	void onGamepadAxis(int gamepad, int axis, int value);
	void onGamepadButton(int gamepad, int buttom, int action);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtScreen create();
};
