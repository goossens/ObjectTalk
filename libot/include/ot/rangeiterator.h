//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "integer.h"
#include "iterator.h"


//
//	OtRangeIteratorClass
//

class OtRangeIteratorClass;
typedef std::shared_ptr<OtRangeIteratorClass> OtRangeIterator;

class OtRangeIteratorClass : public OtIteratorClass {
public:
	// constructors
	OtRangeIteratorClass() = default;
	OtRangeIteratorClass(long f, long t, long i) : from(f), to(t), increment(i), index(f) {}

	// start iterator
	OtObject iterate() { return shared(); }

	// iteration operations
	bool end() {
		if (to > from) {
			return index > to;

		} else {
			return index < to;
		}
	}

	OtObject next() {
		auto result = OtIntegerClass::create(index);
		index += increment;
		return result;
	}

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtRangeIterator create(long from, long to, long increment);

private:
	long from;
	long to;
	long increment;
	long index;
};
