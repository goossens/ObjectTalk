//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtCollection.h"


//
//	OtCollectionClass::getMeta
//

OtType OtCollectionClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtCollectionClass>("Collection", OtObjectClass::getMeta());
	}

	return type;
}
