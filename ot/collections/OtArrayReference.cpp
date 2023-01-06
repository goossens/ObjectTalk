//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
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
		type = OtTypeClass::create<OtArrayReferenceClass>("ArrayReference", OtReferenceClass::getMeta());
		type->set("__deref__", OtFunctionClass::create(&OtArrayReferenceClass::deref));
		type->set("__assign__", OtFunctionClass::create(&OtArrayReferenceClass::assign));
	}

	return type;
}


//
//	OtArrayReferenceClass::create
//

OtArrayReference OtArrayReferenceClass::create(OtArray array, size_t index) {
	OtArrayReference reference = std::make_shared<OtArrayReferenceClass>(array, index);
	reference->setType(getMeta());
	return reference;
}
