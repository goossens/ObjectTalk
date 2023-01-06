//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtDictReference.h"
#include "OtFunction.h"


//
//	OtDictReferenceClass::getMeta
//

OtType OtDictReferenceClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtDictReferenceClass>("DictReference", OtReferenceClass::getMeta());
		type->set("__deref__", OtFunctionClass::create(&OtDictReferenceClass::deref));
		type->set("__assign__", OtFunctionClass::create(&OtDictReferenceClass::assign));
	}

	return type;
}


//
//	OtDictReferenceClass::create
//

OtDictReference OtDictReferenceClass::create(OtDict a, const std::string& i) {
	OtDictReference reference = std::make_shared<OtDictReferenceClass>(a, i);
	reference->setType(getMeta());
	return reference;
}
