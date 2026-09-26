//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtFunction.h"

#include "OtComponentObject.h"


//
//	OtComponentObjectClass::getMeta
//

OtType OtComponentObjectClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtType::create<OtComponentObjectClass>("Component", OtObjectClass::getMeta());
		type->set("getScene", OtFunction::create(&OtComponentObjectClass::getScene));
		type->set("getEntity", OtFunction::create(&OtComponentObjectClass::getEntity));
	}

	return type;
}