//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtComponent.h"


//
//	OtAppObject
//

class OtAppObjectClass;
typedef std::shared_ptr<OtAppObjectClass> OtAppObject;

class OtAppObjectClass : public OtComponentClass {
public:
	// ensure specified component is allowed as a child
	void validateChild(OtComponent child) override;

	// render content
	virtual void render();

	// event handlers
	virtual bool onMouseButton(int button, int action, int mods, float xpos, float ypos) { return false; }
	virtual bool onMouseMove(float xpos, float ypos) { return false; }
	virtual bool onMouseDrag(int button, int mods, float xpos, float ypos) { return false; }
	virtual bool onScrollWheel(float dx, float dy) { return false; }
	virtual bool onKey(int key, int mods) { return false; }
	virtual bool onChar(unsigned int codepoint) { return false; }
	virtual bool onGamepadAxis(int gamepad, int axis, int value) { return false; }
	virtual bool onGamepadButton(int gamepad, int button, int action) { return false; }

	// get type definition
	static OtType getMeta();
};
