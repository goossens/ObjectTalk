//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtFunction.h"

#include "OtMessageComponentObject.h"


//
//	OtMessageComponentObjectClass::getMeta
//

OtType OtMessageComponentObjectClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtType::create<OtMessageComponentObjectClass>("MessageComponent", OtObjectClass::getMeta());

		type->set("show", OtFunction::create(&OtMessageComponentObjectClass::show));
		type->set("hide", OtFunction::create(&OtMessageComponentObjectClass::hide));
	}

	return type;
}
