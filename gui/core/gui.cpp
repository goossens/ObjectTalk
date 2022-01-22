//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "gui.h"


//
//	OtGuiClass::getMeta
//

OtType OtGuiClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtGuiClass>("GUI", OtObjectClass::getMeta());
	}

	return type;
}
