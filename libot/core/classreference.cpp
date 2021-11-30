//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/function.h"
#include "ot/classreference.h"


//
//	OtClassReferenceClass::getMeta
//

OtType OtClassReferenceClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtClassReferenceClass>("ClassReference", OtReferenceClass::getMeta());
		type->set("__deref__", OtFunctionClass::create(&OtClassReferenceClass::deref));
		type->set("__assign__", OtFunctionClass::create(&OtClassReferenceClass::assign));
	}

	return type;
}


//
//	OtClassReferenceClass::create
//

OtClassReference OtClassReferenceClass::create(const std::string& className, const std::string& memberName) {
	OtClassReference reference = std::make_shared<OtClassReferenceClass>(className, memberName);
	reference->setType(getMeta());
	return reference;
}