//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/function.h"

#include "ambientcontroller.h"


//
//	OtAmbientControllerClass::init
//

void OtAmbientControllerClass::init(OtObject object) {
	if (!object->isKindOf("Ambient")) {
		OtExcept("Expected an [Ambient] object, not a [%s]", object->getType()->getName().c_str());
	}

	ambient = object->cast<OtAmbientClass>();
}


//
//	OtAmbientControllerClass::render
//

void OtAmbientControllerClass::render() {
	ambient->renderGUI();
}


//
//	OtAmbientControllerClass::getMeta
//

OtType OtAmbientControllerClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtAmbientControllerClass>("AmbientController", OtWidgetClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtAmbientControllerClass::init));
	}

	return type;
}


//
//	OtAmbientControllerClass::create
//

OtAmbientController OtAmbientControllerClass::create() {
	OtAmbientController ambientcontroller = std::make_shared<OtAmbientControllerClass>();
	ambientcontroller->setType(getMeta());
	return ambientcontroller;
}
