//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtAppObject.h"


//
//	OtAppObjectClass::getMeta
//

OtType OtAppObjectClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtAppObjectClass>("AppObject", OtComponentClass::getMeta());
	}

	return type;
}
