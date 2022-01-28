//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtInternal.h"


//
//	OtInternalClass::getMeta
//

OtType OtInternalClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtInternalClass>("Internal", OtObjectClass::getMeta());
	}

	return type;
}
