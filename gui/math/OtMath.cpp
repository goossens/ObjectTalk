//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtMath.h"


//
//	OtMathClass::getMeta
//

OtType OtMathClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtMathClass>("Math", OtGuiClass::getMeta());
	}

	return type;
}
