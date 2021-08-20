//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/function.h"
#include "ot/vm.h"

#include "theme.h"
#include "view.h"



OtViewClass::~OtViewClass() {
	// clear our scene (it has circular parent/child relationships)
	if (scene) {
		scene->clear();
	}
}


//
//	OtViewClass::setCamera
//

void OtViewClass::setCamera(OtObject cam) {
	// ensure object is a camera
	if (cam->isKindOf("Camera")) {
		camera = cam->cast<OtCameraClass>();

	} else {
		OtExcept("Expected a [Camera] object, not a [%s]", cam->getType()->getName().c_str());
	}
}


//
//	OtViewClass::setScene
//

void OtViewClass::setScene(OtObject scn) {
	// ensure object is a material
	if (scn->isKindOf("Scene")) {
		scene = scn->cast<OtSceneClass>();

	} else {
		OtExcept("Expected a [Scene] object, not a [%s]", scn->getType()->getName().c_str());
	}
}


//
//	OtViewClass::render
//

void OtViewClass::render() {
	// render view if we have a scene and a camera
	if (scene && camera) {
		// determine dimensions
		float vx = x < 0 ? OtTheme::width - (x * OtTheme::width / 100.0) : x * OtTheme::width / 100.0;
		float vy = y < 0 ? OtTheme::height - (y * OtTheme::height / 100.0) : y * OtTheme::height / 100.0;
		float vw = w * OtTheme::width / 100.0;
		float vh = h * OtTheme::height / 100.0;

		// setup camera and viewport
		camera->submit(id, vw / vh);
		bgfx::setViewRect(id, vx, vy, vw, vh);

		// render our scene
		scene->render(id, camera->viewMatrix);
	}
}


//
//	OtViewClass::onMouseButton
//

void OtViewClass::onMouseButton(int button, int action, int mods, double xpos, double ypos) {
	if (has("onMouseButton")) {
		// determine dimensions
		float vx = x < 0 ? OtTheme::width - (x * OtTheme::width / 100.0) : x * OtTheme::width / 100.0;
		float vy = y < 0 ? OtTheme::height - (y * OtTheme::height / 100.0) : y * OtTheme::height / 100.0;
		float vw = w * OtTheme::width / 100.0;
		float vh = h * OtTheme::height / 100.0;

		// calculate local coordinates
		xpos -= vx;
		ypos -= vy;

		// is click in view?
		if (xpos >= 0 && xpos < vw && ypos >= 0 && ypos < vh) {
			OtVM::callMemberFunction(
				shared(), "onMouseButton",
				OtObjectCreate(button), OtObjectCreate(action), OtObjectCreate(mods),
				OtObjectCreate(xpos), OtObjectCreate(ypos)
			);
		}
	}
}


//
//	OtViewClass::onMouseMove
//

void OtViewClass::onMouseMove(int button, double xpos, double ypos) {
	// ensure mouse button is pressed and we have a member function
	if (button != -1 && has("onMouseDrag")) {
		// calculate local coordinates and call member function
		xpos -= x < 0 ? OtTheme::width - (x * OtTheme::width / 100.0) : x * OtTheme::width / 100.0;
		ypos -= y < 0 ? OtTheme::height - (y * OtTheme::height / 100.0) : y * OtTheme::height / 100.0;
		OtVM::callMemberFunction(shared(), "onMouseDrag", OtObjectCreate(button), OtObjectCreate(xpos - xold), OtObjectCreate(ypos - yold));
	}

	xold = xpos;
	yold = ypos;
}


//
//	OtViewClass::onKey
//

void OtViewClass::onKey(int key, int mods) {
	if (has("onKey")) {
		OtVM::callMemberFunction(shared(), "onKey", OtObjectCreate(key), OtObjectCreate(mods));
	}
}


//
//	OtViewClass::onChar
//

void OtViewClass::onChar(unsigned int codepoint) {
}


//
//	OtViewClass::getMeta
//

OtType OtViewClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtViewClass>("View", OtWidgetClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtViewClass::init));
		type->set("setScene", OtFunctionClass::create(&OtViewClass::setScene));
		type->set("setCamera", OtFunctionClass::create(&OtViewClass::setCamera));
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
