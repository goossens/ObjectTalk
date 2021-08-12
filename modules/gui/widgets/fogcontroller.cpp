//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/function.h"

#include "fogcontroller.h"


//
//	OtFogControllerClass::init
//

void OtFogControllerClass::init(OtObject object) {
	if (!object->isKindOf("Fog")) {
		OtExcept("Expected a [Fog] object, not a [%s]", object->getType()->getName().c_str());
	}

	fog = object->cast<OtFogClass>();
}


//
//	OtFogControllerClass::render
//

void OtFogControllerClass::render() {
	fog->renderGUI();
}


//
//	OtFogControllerClass::getMeta
//

OtType OtFogControllerClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtFogControllerClass>("FogController", OtWidgetClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtFogControllerClass::init));
	}

	return type;
}


//
//	OtFogControllerClass::create
//

OtFogController OtFogControllerClass::create() {
	OtFogController fogcontroller = std::make_shared<OtFogControllerClass>();
	fogcontroller->setType(getMeta());
	return fogcontroller;
}
