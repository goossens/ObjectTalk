//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtFunction.h"
#include "OtStringIterator.h"


//
//	OtStringIteratorClass::getMeta
//

OtType OtStringIteratorClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtStringIteratorClass>("StringIterator", OtIteratorClass::getMeta());
		type->set("__end__", OtFunction::create(&OtStringIteratorClass::end));
		type->set("__next__", OtFunction::create(&OtStringIteratorClass::next));
	}

	return type;
}
