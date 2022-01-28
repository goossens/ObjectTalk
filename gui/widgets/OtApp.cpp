//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtFunction.h"
#include "OtVM.h"

#include "OtApp.h"
#include "OtFramework.h"
#include "OtMenubar.h"
#include "OtOS.h"
#include "OtView.h"


//
//	OtAppClass::OtAppClass
//

OtAppClass::OtAppClass() {
	// add ourselves to the GUI framework as a customer
	OtFrameworkClass::instance()->addCustomer(this);
}


//
//	OtAppClass::~OtAppClass
//

OtAppClass::~OtAppClass() {
	// remove ourselves from the GUI framework as a customer
	OtFrameworkClass::instance()->removeCustomer(this);
}


//
//	OtAppClass::validateChild
//

void OtAppClass::validateChild(OtComponent child) {
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
//	OtFrameworkClass::animate
//

OtObject OtAppClass::addAnimation(OtObject object) {
	object->expectKindOf("Animation");
	animations.push_back(object->cast<OtAnimationClass>());
	return shared();
}


//
//	OtFrameworkClass::addSimulation
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
	for (auto& child: children) {
		if (child->isKindOf("Menubar")) {
			return child->cast<OtMenubarClass>()->getHeight();
		}
	}

	return 0;
}


void OtAppClass::onError(OtException e) {
	OtFrameworkClass::instance()->removeCustomer(this);
	OtOSClass::instance()->errorGUI(e);
	std::cout << "Error" << std::endl;
}


//
//	OtAppClass::onSetup
//

void OtAppClass::onSetup() {
	try {
		// call subclass member function (if we have one)
		if (has("setup")) {
			OtVM::instance()->callMemberFunction(shared(), "setup");
		}

	} catch (const OtException& e) {
		onError(e);
	}
}

//
//	OtAppClass::onUpdate
//

void OtAppClass::onUpdate() {
	try {
		// get loop duration
		float loopDuration = OtFrameworkClass::instance()->getLoopDuration();

		// update all animations
		for (int c = animations.size() - 1; c >= 0; c--) {
			if (!animations[c]->step(loopDuration)) {
				animations.erase(animations.begin() + c);
			}
		}

		// update all simulations
		for (auto& simulation : simulations) {
			if (simulation->isRunning()) {
				simulation->step(loopDuration);
			}
		}

		// call subclass member function (if we have one)
		if (has("update")) {
			OtVM::instance()->callMemberFunction(shared(), "update");
		}

		// update all our children
		update();

	} catch (const OtException& e) {
		onError(e);
	}
}


//
//	OtAppClass::onRender
//

void OtAppClass::onRender() {
	try {
		// render all our children
		render();

	} catch (const OtException& e) {
		onError(e);
	}
}


//
//	OtAppClass::onTerminate
//

void OtAppClass::onTerminate() {
	try {
		// call subclass member function (if we have one)
		if (has("terminate")) {
			OtVM::instance()->callMemberFunction(shared(), "terminate");
		}

	} catch (const OtException& e) {
		onError(e);
	}

	// remove all animations and simulations
	animations.clear();
	simulations.clear();

	// remove all children from the app to avoid memory leaks
	// there are circular parent/child relationships
	clear();

	// also remove app instance variables that the user might have added
	unsetAll();
}


//
//	OtAppClass::onMouseButton
//

bool OtAppClass::onMouseButton(int button, int action, int mods, float xpos, float ypos) {
	bool handled = false;

	try {
		// call subclass member function (if we have one)
		if (has("onMouseButton")) {
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

	} catch (const OtException& e) {
		onError(e);
	}

	return handled;
}


//
//	OtAppClass::onMouseMove
//

bool OtAppClass::onMouseMove(float xpos, float ypos) {
	bool handled = false;

	try {
		// call subclass member function (if we have one)
		if (has("onMouseMove")) {
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

	} catch (const OtException& e) {
		onError(e);
	}

	return handled;
}


//
//	OtAppClass::onMouseDrag
//

bool OtAppClass::onMouseDrag(int button, int mods, float xpos, float ypos) {
	bool handled = false;

	try {
		// call subclass member function (if we have one)
		if (has("onMouseDrag")) {
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

	} catch (const OtException& e) {
		onError(e);
	}

	return handled;
}


//
//	OtAppClass::onScrollWheel
//

bool OtAppClass::onScrollWheel(float dx, float dy){
	bool handled = false;

	try {
		// call subclass member function (if we have one)
		if (has("onScrollWheel")) {
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

	} catch (const OtException& e) {
		onError(e);
	}

	return handled;
}


//
//	OtAppClass::onKey
//

bool OtAppClass::onKey(int key, int mods) {
	bool handled = false;

	try {
		// call subclass member function (if we have one)
		if (has("onKey")) {
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

	} catch (const OtException& e) {
		onError(e);
	}

	return handled;
}


//
//	OtAppClass::onChar
//

bool OtAppClass::onChar(unsigned int codepoint) {
	bool handled = false;

	try {
		// call subclass member function (if we have one)
		if (has("onChar")) {
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

	} catch (const OtException& e) {
		onError(e);
	}

	return handled;
}


//
//	OtAppClass::onGamepadAxis
//

bool OtAppClass::onGamepadAxis(int gamepad, int axis, int value) {
	bool handled = false;
	try {
		// call subclass member function (if we have one)
		if (has("onGamepadAxis")) {
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

	} catch (const OtException& e) {
		onError(e);
	}

	return handled;
}


//
//	OtAppClass::onGamepadButton
//

bool OtAppClass::onGamepadButton(int gamepad, int button, int action) {
	bool handled = false;

	try {
		// call subclass member function (if we have one)
		if (has("onGamepadButton")) {
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

	} catch (const OtException& e) {
		onError(e);
	}

	return handled;
}


//
//	OtAppClass::getMeta
//

OtType OtAppClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtAppClass>("App", OtWidgetClass::getMeta());

		type->set("addAnimation", OtFunctionClass::create(&OtAppClass::addAnimation));
		type->set("addSimulation", OtFunctionClass::create(&OtAppClass::addSimulation));

		type->set("getWidth", OtFunctionClass::create(&OtAppClass::getWidth));
		type->set("getHeight", OtFunctionClass::create(&OtAppClass::getHeight));
	}

	return type;
}


//
//	OtAppClass::create
//

OtApp OtAppClass::create() {
	OtApp screen = std::make_shared<OtAppClass>();
	screen->setType(getMeta());
	return screen;
}