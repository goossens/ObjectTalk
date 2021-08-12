//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/function.h"

#include "cameracontroller.h"


//
//	OtCameraControllerClass::init
//

void OtCameraControllerClass::init(OtObject object) {
	if (!object->isKindOf("Camera")) {
		OtExcept("Expected a [Camera] object, not a [%s]", object->getType()->getName().c_str());
	}

	camera = object->cast<OtCameraClass>();
}


//
//	OtCameraControllerClass::render
//

void OtCameraControllerClass::render() {
	camera->renderGUI();
}


//
//	OtCameraControllerClass::getMeta
//

OtType OtCameraControllerClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtCameraControllerClass>("CameraController", OtWidgetClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtCameraControllerClass::init));
	}

	return type;
}


//
//	OtCameraControllerClass::create
//

OtCameraController OtCameraControllerClass::create() {
	OtCameraController cameracontroller = std::make_shared<OtCameraControllerClass>();
	cameracontroller->setType(getMeta());
	return cameracontroller;
}
