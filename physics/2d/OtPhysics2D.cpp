//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtPhysics2D.h"


//
//	OtPhysics2DClass::getMeta
//

OtType OtPhysics2DClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtPhysics2DClass>("Physics2D", OtObjectClass::getMeta());
	}

	return type;
}
