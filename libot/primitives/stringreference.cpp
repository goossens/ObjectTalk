//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/function.h"
#include "ot/stringreference.h"


//
//	OtStringReferenceClass::getMeta
//

OtType OtStringReferenceClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtStringReferenceClass>("StringReference", OtReferenceClass::getMeta());
		type->set("__deref__", OtFunctionClass::create(&OtStringReferenceClass::deref));
		type->set("__assign__", OtFunctionClass::create(&OtStringReferenceClass::assign));
	}

	return type;
}


//
//	OtStringReferenceClass::create
//

OtStringReference OtStringReferenceClass::create(OtString string, size_t index) {
	OtStringReference reference = std::make_shared<OtStringReferenceClass>(string, index);
	reference->setType(getMeta());
	return reference;
}
