//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/exception.h"
#include "ot/function.h"
#include "ot/throw.h"


//
//	OtThrowClass::call
//

void OtThrowClass::call(const std::string& error) {
	OtExcept(error);
}


//
//	OtThrowClass::getMeta
//

OtType OtThrowClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtThrowClass>("Throw", OtInternalClass::getMeta());
		type->set("__call__", OtFunctionClass::create(&OtThrowClass::call));
	}

	return type;
}


//
//	OtThrowClass::create
//

OtThrow OtThrowClass::create() {
	OtThrow thrw = std::make_shared<OtThrowClass>();
	thrw->setType(getMeta());
	return thrw;
}
