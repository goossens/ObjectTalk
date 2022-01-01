//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/function.h"
#include "ot/objectreference.h"


//
//	OtObjectReferenceClass::getMeta
//

OtType OtObjectReferenceClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtObjectReferenceClass>("ObjectReference", OtReferenceClass::getMeta());
		type->set("__deref__", OtFunctionClass::create(&OtObjectReferenceClass::deref));
		type->set("__assign__", OtFunctionClass::create(&OtObjectReferenceClass::assign));
	}

	return type;
}


//
//	OtObjectReferenceClass::getMeta
//

OtObjectReference OtObjectReferenceClass::create(OtObject o, const std::string& m) {
	OtObjectReference reference = std::make_shared<OtObjectReferenceClass>(o, m);
	reference->setType(getMeta());
	return reference;
}
