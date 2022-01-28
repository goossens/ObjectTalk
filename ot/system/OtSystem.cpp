//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtSystem.h"


//
//	OtSystemClass::getMeta
//

OtType OtSystemClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtSystemClass>("System", OtObjectClass::getMeta());
	}

	return type;
}
