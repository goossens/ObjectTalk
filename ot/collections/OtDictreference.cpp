//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
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
		type = OtType::create<OtDictReferenceClass>("DictReference", OtReferenceClass::getMeta());
		type->set("__deref__", OtFunction::create(&OtDictReferenceClass::deref));
		type->set("__assign__", OtFunction::create(&OtDictReferenceClass::assign));
	}

	return type;
}
