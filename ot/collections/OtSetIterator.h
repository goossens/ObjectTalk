//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>

#include "OtSet.h"
#include "OtIterator.h"


//
//	OtArrayIteratorClass
//

class OtSetIteratorClass;
using OtSetIterator = OtObjectPointer<OtSetIteratorClass>;

class OtSetIteratorClass : public OtIteratorClass {
public:
	// constructors
	OtSetIteratorClass() = default;

	OtSetIteratorClass(OtSet s) : set(s) {
		iterator = set->set.begin();
	}

	// iteration operations
	bool end() { return iterator == set->set.end(); }
	OtObject next() { return *iterator++; }

	// get type definition
	static OtType getMeta();

private:
	OtSet set;
	std::set<OtObject>::iterator iterator;
};
