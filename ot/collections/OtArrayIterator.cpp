//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtArrayIterator.h"
#include "OtFunction.h"


//
//	OtArrayIteratorClass::getMeta(
//

OtType OtArrayIteratorClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtArrayIteratorClass>("ArrayIterator", OtIteratorClass::getMeta());
		type->set("__end__", OtFunction::create(&OtArrayIteratorClass::end));
		type->set("__next__", OtFunction::create(&OtArrayIteratorClass::next));
	}

	return type;
}
