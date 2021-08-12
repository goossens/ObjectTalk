//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/function.h"

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
//	OtViewClass::getMeta
//

OtType OtViewClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtViewClass>("View", OtComponentClass::getMeta());
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
