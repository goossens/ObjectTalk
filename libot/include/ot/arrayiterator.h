//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>

#include "array.h"
#include "iterator.h"


//
//	OtArrayIteratorClass
//

class OtArrayIteratorClass;
typedef std::shared_ptr<OtArrayIteratorClass> OtArrayIterator;

class OtArrayIteratorClass : public OtIteratorClass {
public:
	// constructors
	OtArrayIteratorClass() = default;
	OtArrayIteratorClass(OtArray a) : array(a) {}

	// iteration operations
	bool end() { return index == array->size(); }
	OtObject next() { return array->getEntry(index++); }

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtArrayIterator create(OtArray a);

private:
	OtArray array;
	size_t index {0};
};