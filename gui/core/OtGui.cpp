//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtClass.h"

#include "OtGui.h"


//
//	OtGuiClass::getMeta
//

OtType OtGuiClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtGuiClass>("GUI", OtObjectClass::getMeta());
	}

	return type;
}
