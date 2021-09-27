//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/function.h"
#include "ot/arrayreference.h"


//
//	OtArrayReferenceClass::getMeta
//

OtType OtArrayReferenceClass::getMeta() {
	static OtType type = nullptr;

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
