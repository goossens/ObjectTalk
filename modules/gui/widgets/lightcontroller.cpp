//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/function.h"

#include "lightcontroller.h"


//
//	OtLightControllerClass::init
//

void OtLightControllerClass::init(OtObject object) {
	if (!object->isKindOf("Light")) {
		OtExcept("Expected a [Light] object, not a [%s]", object->getType()->getName().c_str());
	}

	light = object->cast<OtLightClass>();
}


//
//	OtLightControllerClass::render
//

void OtLightControllerClass::render() {
	light->renderGUI();
}


//
//	OtLightControllerClass::getMeta
//

OtType OtLightControllerClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtLightControllerClass>("LightController", OtWidgetClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtLightControllerClass::init));
	}

	return type;
}


//
//	OtLightControllerClass::create
//

OtLightController OtLightControllerClass::create() {
	OtLightController lightcontroller = std::make_shared<OtLightControllerClass>();
	lightcontroller->setType(getMeta());
	return lightcontroller;
}
