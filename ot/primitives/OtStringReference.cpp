//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtFunction.h"
#include "OtStringReference.h"


//
//	OtStringReferenceClass::getMeta
//

OtType OtStringReferenceClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtStringReferenceClass>("StringReference", OtReferenceClass::getMeta());
		type->set("__deref__", OtFunction::create(&OtStringReferenceClass::deref));
		type->set("__assign__", OtFunction::create(&OtStringReferenceClass::assign));
	}

	return type;
}
