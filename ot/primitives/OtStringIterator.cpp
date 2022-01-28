//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
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
		type = OtTypeClass::create<OtStringIteratorClass>("StringIterator", OtIteratorClass::getMeta());
		type->set("__end__", OtFunctionClass::create(&OtStringIteratorClass::end));
		type->set("__next__", OtFunctionClass::create(&OtStringIteratorClass::next));
	}

	return type;
}

//
//	OtStringIteratorClass::create
//

OtStringIterator OtStringIteratorClass::create(OtString string) {
	OtStringIterator iterator = std::make_shared<OtStringIteratorClass>(string);
	iterator->setType(getMeta());
	return iterator;
}