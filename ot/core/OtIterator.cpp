//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtIterator.h"


//
//	OtIteratorClass::getMeta
//

OtType OtIteratorClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtIteratorClass>("Iterator", OtInternalClass::getMeta());
	}

	return type;
}
