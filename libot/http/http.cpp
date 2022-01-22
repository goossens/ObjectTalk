//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/http.h"


//
//	OtHttpClass::getMeta
//

OtType OtHttpClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtHttpClass>("Http", OtObjectClass::getMeta());
	}

	return type;
}
