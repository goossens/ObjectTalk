//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtStream.h"


//
//	OtStreamClass::getMeta
//

OtType OtStreamClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtStreamClass>("Stream", OtObjectClass::getMeta());
	}

	return type;
}
