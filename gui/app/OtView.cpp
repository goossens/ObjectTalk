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

#include "OtFramework.h"
#include "OtView.h"


//
//	OtViewClass::~OtViewClass
//

OtViewClass::~OtViewClass() {
	// clear our scene (it has circular parent/child relationships)
	if (scene) {
		scene->clear();
		scene = nullptr;
	}
}


//
//	OtViewClass::setScreenArea
//

OtObject OtViewClass::setScreenArea(int _x, int _y, int _w, int _h) {
	x = _x;
	y = _y;
	w = _w;
	h = _h;
	return shared();
}


//
//	OtViewClass::setCamera
//

OtObject OtViewClass::setCamera(OtObject object) {
	object->expectKindOf("Camera");
	camera = object->cast<OtCameraClass>();
	return shared();
}


//
//	OtViewClass::setScene
//

OtObject OtViewClass::setScene(OtObject object) {
	object->expectKindOf("Scene");
	scene = object->cast<OtSceneClass>();
	return shared();
}


//
//	OtViewClass::update
//

void OtViewClass::update() {
	// update view if we have a scene and a camera
	if (scene && camera) {
		// determine dimensions
		OtFramework framework = OtFrameworkClass::instance();
		float sw = framework->getWidth();
		float sh = framework->getHeight();

		vx = x < 0 ? sw - (x * sw / 100.0) : x * sw / 100.0;
		vy = y < 0 ? sw - (y * sh / 100.0) : y * sh / 100.0;
		vw = w * sw / 100.0;
		vh = h * sh / 100.0;

		// update scene
		scene->update(camera, vx, vy, vw, vh);
	}
}


//
//	OtViewClass::render
//

void OtViewClass::render() {
	// render view if we have a scene and a camera
	if (scene && camera) {
		scene->render();
	}
}


//
//	OtViewClass::onMouseButton
//

bool OtViewClass::onMouseButton(int button, int action, int mods, float xpos, float ypos) {
	if (has("onMouseButton")) {
		// determine dimensions
		OtFramework framework = OtFrameworkClass::instance();
		float sw = framework->getWidth();
		float sh = framework->getHeight();

		float vx = x < 0 ? sw - (x * sw / 100.0) : x * sw / 100.0;
		float vy = y < 0 ? sw - (y * sh / 100.0) : y * sh / 100.0;
		float vw = w * sw / 100.0;
		float vh = h * sh / 100.0;

		// calculate local coordinates
		xpos -= vx;
		ypos -= vy;

		// is click in view?
		if (xpos >= 0 && xpos < vw && ypos >= 0 && ypos < vh) {
			OtVM::instance()->callMemberFunction(
				shared(), "onMouseButton",
				OtObjectCreate(button), OtObjectCreate(action), OtObjectCreate(mods),
				OtObjectCreate(xpos), OtObjectCreate(ypos)
			);
		}

		return true;

	} else {
		return false;
	}
}


//
//	OtViewClass::onMouseMove
//

bool OtViewClass::onMouseMove(float xpos, float ypos) {
	// calculate local coordinates
	OtFramework framework = OtFrameworkClass::instance();
	float sw = framework->getWidth();
	float sh = framework->getHeight();

	xpos -= x < 0 ? sw - (x * sw / 100.0) : x * sw / 100.0;
	ypos -= y < 0 ? sh - (y * sh / 100.0) : y * sh / 100.0;

	bool handled = false;

	if (has("onMouseMove")) {
		// call member function
		OtVM::instance()->callMemberFunction(shared(), "onMouseMove", OtObjectCreate(xpos), OtObjectCreate(ypos));
		handled = true;
	}

	xold = xpos;
	yold = ypos;
	return handled;
}


//
//	OtViewClass::onMouseDrag
//

bool OtViewClass::onMouseDrag(int button, int mods, float xpos, float ypos) {
	// calculate local coordinates
	OtFramework framework = OtFrameworkClass::instance();
	float sw = framework->getWidth();
	float sh = framework->getHeight();

	xpos -= x < 0 ? sw - (x * sw / 100.0) : x * sw / 100.0;
	ypos -= y < 0 ? sh - (y * sh / 100.0) : y * sh / 100.0;

	bool handled = false;

	// only process if camera doesn't want it and  we have a member function
	if (!camera->onMouseDrag(button, mods, xpos - xold, ypos - yold) && has("onMouseDrag")) {
		OtVM::instance()->callMemberFunction(shared(), "onMouseDrag", OtObjectCreate(button), OtObjectCreate(mods), OtObjectCreate(xpos - xold), OtObjectCreate(ypos - yold));
		handled = true;
	}

	xold = xpos;
	yold = ypos;
	return handled;
}


//
//	OtViewClass::onScrollWheel
//

bool OtViewClass::onScrollWheel(float dx, float dy) {
	// only process if camera doesn't want event and we have a member function
	if (camera->onScrollWheel(dx, dy)) {
		return true;

	} else if (has("onScrollWheel")) {
		OtVM::instance()->callMemberFunction(shared(), "onScrollWheel", OtObjectCreate(dx), OtObjectCreate(dy));
		return true;

	} else {
		return false;
	}
}


//
//	OtViewClass::onKey
//

bool OtViewClass::onKey(int key, int mods) {
	// only process if camera doesn't want event and we have a member function
	if (camera->onKey(key, mods)) {
		return true;

	} else if (has("onKey")) {
		OtVM::instance()->callMemberFunction(shared(), "onKey", OtObjectCreate(key), OtObjectCreate(mods));
		return true;

	} else {
		return false;
	}
}


//
//	OtViewClass::onChar
//

bool OtViewClass::onChar(unsigned int codepoint) {
	if (has("onChar")) {
		OtVM::instance()->callMemberFunction(shared(), "onChar", OtObjectCreate((int) codepoint));
		return true;

	} else {
		return false;
	}
}


//
//	OtViewClass::getMeta
//

OtType OtViewClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtViewClass>("View", OtAppObjectClass::getMeta());

		type->set("setScreenArea", OtFunctionClass::create(&OtViewClass::setScreenArea));
		type->set("setCamera", OtFunctionClass::create(&OtViewClass::setCamera));
		type->set("setScene", OtFunctionClass::create(&OtViewClass::setScene));

		type->set("getCamera", OtFunctionClass::create(&OtViewClass::getCamera));
		type->set("getScene", OtFunctionClass::create(&OtViewClass::getScene));
	}

	return type;
}


//
//	OtViewClass::create
//

OtView OtViewClass::create() {
	OtView view = std::make_shared<OtViewClass>();
	view->setType(getMeta());
	return view;
}
