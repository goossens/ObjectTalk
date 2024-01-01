//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
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
		type = OtType::create<OtPathIteratorClass>("PathIterator", OtIteratorClass::getMeta());
		type->set("__end__", OtFunction::create(&OtPathIteratorClass::end));
		type->set("__next__", OtFunction::create(&OtPathIteratorClass::next));
	}

	return type;
}
