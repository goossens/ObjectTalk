//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtFunction.h"
#include "OtRangeIterator.h"


//
//	OtRangeIteratorClass::getMeta(
//

OtType OtRangeIteratorClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtRangeIteratorClass>("RangeIterator", OtIteratorClass::getMeta());
		type->set("__iter__", OtFunction::create(&OtRangeIteratorClass::iterate));
		type->set("__end__", OtFunction::create(&OtRangeIteratorClass::end));
		type->set("__next__", OtFunction::create(&OtRangeIteratorClass::next));
	}

	return type;
}
