//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtFunction.h"
#include "OtSetIterator.h"


//
//	OtSetIteratorClass::getMeta(
//

OtType OtSetIteratorClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtSetIteratorClass>("SetIterator", OtIteratorClass::getMeta());
		type->set("__end__", OtFunction::create(&OtSetIteratorClass::end));
		type->set("__next__", OtFunction::create(&OtSetIteratorClass::next));
	}

	return type;
}
