//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtFunction.h"
#include "OtPathIterator.h"


//
//	OtPathIteratorClass::getMeta
//

OtType OtPathIteratorClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtPathIteratorClass>("PathIterator", OtIteratorClass::getMeta());
		type->set("__end__", OtFunctionClass::create(&OtPathIteratorClass::end));
		type->set("__next__", OtFunctionClass::create(&OtPathIteratorClass::next));
	}

	return type;
}


//
//	OtPathIteratorClass::create
//

OtPathIterator OtPathIteratorClass::create(OtPath path) {
	OtPathIterator iterator = std::make_shared<OtPathIteratorClass>(path);
	iterator->setType(getMeta());
	return iterator;
}