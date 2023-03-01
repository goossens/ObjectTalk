//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtFunction.h"
#include "OtObjectReference.h"


//
//	OtObjectReferenceClass::getMeta
//

OtType OtObjectReferenceClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtObjectReferenceClass>("ObjectReference", OtReferenceClass::getMeta());
		type->set("__deref__", OtFunction::create(&OtObjectReferenceClass::deref));
		type->set("__assign__", OtFunction::create(&OtObjectReferenceClass::assign));
	}

	return type;
}
