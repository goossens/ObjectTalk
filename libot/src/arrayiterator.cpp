//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/function.h"
#include "ot/arrayiterator.h"


//
//	OtArrayIteratorClass::getMeta(
//

OtType OtArrayIteratorClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtArrayIteratorClass>("ArrayIterator", OtIteratorClass::getMeta());
		type->set("__end__", OtFunctionClass::create(&OtArrayIteratorClass::end));
		type->set("__next__", OtFunctionClass::create(&OtArrayIteratorClass::next));
	}

	return type;
}

//
//	OtArrayIteratorClass::create
//

OtArrayIterator OtArrayIteratorClass::create(OtArray a) {
	OtArrayIterator iterator = std::make_shared<OtArrayIteratorClass>(a);
	iterator->setType(getMeta());
	return iterator;
}
