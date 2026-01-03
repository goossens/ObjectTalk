//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtArrayReference.h"
#include "OtFunction.h"


//
//	OtArrayReferenceClass::getMeta
//

OtType OtArrayReferenceClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtArrayReferenceClass>("ArrayReference", OtReferenceClass::getMeta());
		type->set("__deref__", OtFunction::create(&OtArrayReferenceClass::deref));
		type->set("__assign__", OtFunction::create(&OtArrayReferenceClass::assign));
	}

	return type;
}
