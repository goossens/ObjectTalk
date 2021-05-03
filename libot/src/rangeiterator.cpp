//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/function.h"
#include "ot/rangeiterator.h"


//
//	OtRangeIteratorClass::getMeta(
//

OtType OtRangeIteratorClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtRangeIteratorClass>("RangeIterator", OtIteratorClass::getMeta());
		type->set("__iter__", OtFunctionClass::create(&OtRangeIteratorClass::iterate));
		type->set("__end__", OtFunctionClass::create(&OtRangeIteratorClass::end));
		type->set("__next__", OtFunctionClass::create(&OtRangeIteratorClass::next));
	}

	return type;
}

//
//	OtRangeIteratorClass::create
//

OtRangeIterator OtRangeIteratorClass::create(long from, long to, long increment) {
	OtRangeIterator iterator = std::make_shared<OtRangeIteratorClass>(from, to, increment);
	iterator->setType(getMeta());
	return iterator;
}
