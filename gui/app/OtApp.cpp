//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtFunction.h"
#include "OtVM.h"

#include "OtApp.h"
#include "OtAppMenubar.h"
#include "OtFramework.h"
#include "OtGui.h"
#include "OtView.h"


//
//	OtAppClass::validateChild
//

void OtAppClass::validateChild(OtNode child) {
	if (!(child->isKindOf("AppObject"))) {
		OtExcept("An [App] can only have [AppObject] instances as children, not a [%s]", child->getType()->getName().c_str());
	}
}


//
//	OtAppClass::getWidth
//

int OtAppClass::getWidth() {
	return OtFrameworkClass::instance()->getWidth();
}


//
//	OtAppClass::getHeight
//

int OtAppClass::getHeight() {
	return OtFrameworkClass::instance()->getHeight();
}


//
//	OtAppClass::animate
//

OtObject OtAppClass::addAnimation(OtObject object) {
	object->expectKindOf("Animation");
	animations.push_back(object->cast<OtAnimationClass>());
	return shared();
}


//
//	OtAppClass::addSimulation
//

OtObject OtAppClass::addSimulation(OtObject object) {
	object->expectKindOf("Simulation");
	simulations.push_back(object->cast<OtSimulationClass>());
	return shared();
}


//
//	OtAppClass::getMenubarHeight
//

size_t OtAppClass::getMenubarHeight() {
	for (auto& child : children) {
		if (child->isKindOf("AppMenubar")) {
			return child->cast<OtAppMenubarClass>()->getHeight();
		}
	}

	return 0;
}


//
//	OtAppClass::onSetup
//

void OtAppClass::onSetup() {
	// call subclass member function (if we have one)
	if (hasByName("setup")) {
		OtVM::instance()->callMemberFunction(shared(), "setup");
	}
}

//
//	OtAppClass::onUpdate
//

void OtAppClass::onUpdate() {
	// get loop duration
	float loopDuration = OtFrameworkClass::instance()->getLoopDuration();

	// update all animations (and remove them from the list if they are done)
	animations.erase(std::remove_if(animations.begin(), animations.end(), [loopDuration] (OtAnimation animation) {
		return !animation->step(loopDuration);
	}), animations.end());

	// update all simulations
	for (auto& simulation : simulations) {
		if (simulation->isRunning()) {
			simulation->step(loopDuration);
		}
	}

	// call subclass member function (if we have one)
	if (hasByName("update")) {
		OtVM::instance()->callMemberFunction(shared(), "update");
	}
}


//
//	OtAppClass::onRender
//

void OtAppClass::onRender() {
	// render all our children
	for (auto const& child : children) {
		if (child->isEnabled()) {
			child->cast<OtAppObjectClass>()->render();
		}
	}
}


//
//	OtAppClass::onTerminate
//

void OtAppClass::onTerminate() {
	// call subclass member function (if we have one)
	if (hasByName("terminate")) {
		OtVM::instance()->callMemberFunction(shared(), "terminate");
	}

	// remove all animations and simulations
	animations.clear();
	simulations.clear();

	// remove all children from the app to avoid memory leaks
	clear();

	// also remove app instance variables that the user might have added
	unsetAll();
}


//
//	OtAppClass::onMouseButton
//

bool OtAppClass::onMouseButton(int button, int action, int mods, float xpos, float ypos) {
	bool handled = false;

	// call subclass member function (if we have one)
	if (hasByName("onMouseButton")) {
		OtVM::instance()->callMemberFunction(
			shared(),
			"onMouseButton",
			OtObjectCreate(button),
			OtObjectCreate(action),
			OtObjectCreate(mods),
			OtObjectCreate(xpos),
			OtObjectCreate(ypos));

		handled = true;

	} else {
		// give each of our children a chance to handle the event
		for (auto it = children.begin(); !handled && it != children.end(); it++) {
			handled = (*it)->cast<OtAppObjectClass>()->onMouseButton(button, action, mods, xpos, ypos);
		}
	}

	return handled;
}


//
//	OtAppClass::onMouseMove
//

bool OtAppClass::onMouseMove(float xpos, float ypos) {
	bool handled = false;

	// call subclass member function (if we have one)
	if (hasByName("onMouseMove")) {
		OtVM::instance()->callMemberFunction(
			shared(),
			"onMouseMove",
			OtObjectCreate(xpos),
			OtObjectCreate(ypos));

		handled = true;

	} else {
		// give each of our children a chance to handle the event
		for (auto it = children.begin(); !handled && it != children.end(); it++) {
			handled = (*it)->cast<OtAppObjectClass>()->onMouseMove(xpos, ypos);
		}
	}

	return handled;
}


//
//	OtAppClass::onMouseDrag
//

bool OtAppClass::onMouseDrag(int button, int mods, float xpos, float ypos) {
	bool handled = false;

	// call subclass member function (if we have one)
	if (hasByName("onMouseDrag")) {
		OtVM::instance()->callMemberFunction(
			shared(),
			"onMouseDrag",
			OtObjectCreate(button),
			OtObjectCreate(mods),
			OtObjectCreate(xpos),
			OtObjectCreate(ypos));

		handled = true;

	} else {
		// give each of our children a chance to handle the event
		for (auto it = children.begin(); !handled && it != children.end(); it++) {
			handled = (*it)->cast<OtAppObjectClass>()->onMouseDrag(button, mods, xpos, ypos);
		}
	}

	return handled;
}


//
//	OtAppClass::onScrollWheel
//

bool OtAppClass::onScrollWheel(float dx, float dy){
	bool handled = false;

	// call subclass member function (if we have one)
	if (hasByName("onScrollWheel")) {
		OtVM::instance()->callMemberFunction(
			shared(),
			"onScrollWheel",
			OtObjectCreate(dx),
			OtObjectCreate(dy));

		handled = true;

	} else {
		// give each of our children a chance to handle the event
		for (auto it = children.begin(); !handled && it != children.end(); it++) {
			handled = (*it)->cast<OtAppObjectClass>()->onScrollWheel(dx, dy);
		}
	}

	return handled;
}


//
//	OtAppClass::onKey
//

bool OtAppClass::onKey(int key, int mods) {
	bool handled = false;

	// call subclass member function (if we have one)
	if (hasByName("onKey")) {
		OtVM::instance()->callMemberFunction(
			shared(),
			"onKey",
			OtObjectCreate(key),
			OtObjectCreate(mods));

		handled = true;

	} else {
		// give each of our children a chance to handle the event
		for (auto it = children.begin(); !handled && it != children.end(); it++) {
			handled = (*it)->cast<OtAppObjectClass>()->onKey(key, mods);
		}
	}

	return handled;
}


//
//	OtAppClass::onChar
//

bool OtAppClass::onChar(unsigned int codepoint) {
	bool handled = false;

	// call subclass member function (if we have one)
	if (hasByName("onChar")) {
		OtVM::instance()->callMemberFunction(
			shared(),
			"onChar",
			OtObjectCreate((int) codepoint));

		handled = true;

	} else {
		// give each of our children a chance to handle the event
		for (auto it = children.begin(); !handled && it != children.end(); it++) {
			handled = (*it)->cast<OtAppObjectClass>()->onChar(codepoint);
		}
	}

	return handled;
}


//
//	OtAppClass::onGamepadAxis
//

bool OtAppClass::onGamepadAxis(int gamepad, int axis, int value) {
	bool handled = false;

	// call subclass member function (if we have one)
	if (hasByName("onGamepadAxis")) {
		OtVM::instance()->callMemberFunction(
			shared(),
			"onGamepadAxis",
			OtObjectCreate(gamepad),
			OtObjectCreate(axis),
			OtObjectCreate(value));

		handled = true;

	} else {
		// give each of our children a chance to handle the event
		for (auto it = children.begin(); !handled && it != children.end(); it++) {
			handled = (*it)->cast<OtAppObjectClass>()->onGamepadAxis(gamepad, axis, value);
		}
	}

	return handled;
}


//
//	OtAppClass::onGamepadButton
//

bool OtAppClass::onGamepadButton(int gamepad, int button, int action) {
	bool handled = false;

	// call subclass member function (if we have one)
	if (hasByName("onGamepadButton")) {
		OtVM::instance()->callMemberFunction(
			shared(),
			"onGamepadButton",
			OtObjectCreate(gamepad),
			OtObjectCreate(button),
			OtObjectCreate(action));

		handled = true;

	} else {
		// give each of our children a chance to handle the event
		for (auto it = children.begin(); !handled && it != children.end(); it++) {
			handled = (*it)->cast<OtAppObjectClass>()->onGamepadButton(gamepad, button, action);
		}
	}

	return handled;
}


//
//	OtAppClass::run
//

void OtAppClass::run() {
	OtFrameworkClass::instance()->run(this);
}


//
//	OtAppClass::getMeta
//

OtType OtAppClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtAppClass>("App", OtNodeClass::getMeta());

		type->set("addAnimation", OtFunctionClass::create(&OtAppClass::addAnimation));
		type->set("addSimulation", OtFunctionClass::create(&OtAppClass::addSimulation));

		type->set("getWidth", OtFunctionClass::create(&OtAppClass::getWidth));
		type->set("getHeight", OtFunctionClass::create(&OtAppClass::getHeight));

		type->set("run", OtFunctionClass::create(&OtAppClass::run));
	}

	return type;
}


//
//	OtAppClass::create
//

OtApp OtAppClass::create() {
	OtApp app = std::make_shared<OtAppClass>();
	app->setType(getMeta());
	return app;
}
