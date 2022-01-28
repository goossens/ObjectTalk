//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtAnimation.h"
#include "OtCustomer.h"
#include "OtSimulation.h"
#include "OtWidget.h"


//
//	OtAppClass
//

class OtAppClass;
typedef std::shared_ptr<OtAppClass> OtApp;

class OtAppClass : public OtWidgetClass, public OtCustomer {
public:
	// constructor/destructor
	OtAppClass();
	~OtAppClass();

	// ensure specified component is allowed as a child
	void validateChild(OtComponent child) override;

	// get frame dimensions
	int getWidth();
	int getHeight();

	// add an animation
	OtObject addAnimation(OtObject animation);

	// add a simulation
	OtObject addSimulation(OtObject simulation);

	// get menubar height
	size_t getMenubarHeight();

	// handler to deal with script errors
	void onError(OtException e);

	// lifetime events
	void onSetup() override;
	void onUpdate() override;
	void onRender() override;
	void onTerminate() override;

	// mouse and keyboard events
	bool onMouseButton(int button, int action, int mods, float xpos, float ypos) override;
	bool onMouseMove(float xpos, float ypos) override;
	bool onMouseDrag(int button, int mods, float xpos, float ypos) override;
	bool onScrollWheel(float dx, float dy) override;
	bool onKey(int key, int mods) override;
	bool onChar(unsigned int codepoint) override;
	bool onGamepadAxis(int gamepad, int axis, int value) override;
	bool onGamepadButton(int gamepad, int buttom, int action) override;

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtApp create();

private:
	// animations
	std::vector<OtAnimation> animations;

	// simulations
	std::vector<OtSimulation> simulations;
};
