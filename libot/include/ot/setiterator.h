//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>

#include "set.h"
#include "iterator.h"


//
//	OtArrayIteratorClass
//

class OtSetIteratorClass;
typedef std::shared_ptr<OtSetIteratorClass> OtSetIterator;

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

	// create a new object
	static OtSetIterator create(OtSet set);

private:
	OtSet set;
	std::set<OtObject>::iterator iterator;
};
