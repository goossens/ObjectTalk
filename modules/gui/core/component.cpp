//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>

#include "ot/function.h"

#include "component.h"


//
//	OtComponentClass::getMeta
//

OtType OtComponentClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtComponentClass>("Component", OtGuiClass::getMeta());
		type->set("enable", OtFunctionClass::create(&OtComponentClass::enable));
		type->set("disable", OtFunctionClass::create(&OtComponentClass::disable));
		type->set("setEnabled", OtFunctionClass::create(&OtComponentClass::setEnabled));
		type->set("isEnabled", OtFunctionClass::create(&OtComponentClass::isEnabled));
	}

	return type;
}


//
//	OtComponentClass::create
//

OtComponent OtComponentClass::create() {
	OtComponent component = std::make_shared<OtComponentClass>();
	component->setType(getMeta());
	return component;
}
