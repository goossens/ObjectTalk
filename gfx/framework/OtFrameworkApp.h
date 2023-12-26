//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	OtFrameworkApp class
//

class OtFrameworkApp {
public:
	// destructor
	virtual inline ~OtFrameworkApp() {}

	// event handlers
	virtual inline void onSetup() {}
	virtual inline void onRender() {}
	virtual inline void onTerminate() {}
	virtual inline bool onCanQuit() { return true; }

	virtual inline bool onMouseButton(int button, int action, int mods, float xpos, float ypos) { return false; }
	virtual inline bool onMouseMove(float xpos, float ypos) { return false; }
	virtual inline bool onMouseDrag(int button, int mods, float xpos, float ypos) { return false; }
	virtual inline bool onScrollWheel(float dx, float dy) { return false; }
	virtual inline bool onKey(int key, int mods) { return false; }
	virtual inline bool onChar(unsigned int codepoint) { return false; }
	virtual inline bool onGamepadAxis(int gamepad, int axis, int value) { return false; }
	virtual inline bool onGamepadButton(int gamepad, int buttom, int action) { return false; }
};
