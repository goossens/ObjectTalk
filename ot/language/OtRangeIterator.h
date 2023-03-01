//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtInteger.h"
#include "OtIterator.h"


//
//	OtRangeIteratorClass
//

class OtRangeIteratorClass;
using OtRangeIterator = OtObjectPointer<OtRangeIteratorClass>;

class OtRangeIteratorClass : public OtIteratorClass {
public:
	// constructors
	OtRangeIteratorClass() = default;
	OtRangeIteratorClass(int64_t f, int64_t t, int64_t i) : from(f), to(t), increment(i), index(f) {}

	// start iterator
	OtObject iterate() { return OtRangeIterator(this); }

	// iteration operations
	bool end() {
		return (to > from) ? (index > to) : (index < to);
	}

	int64_t next() {
		auto result = index; index += increment;
		return result;
	}

	// get type definition
	static OtType getMeta();

private:
	int64_t from;
	int64_t to;
	int64_t increment;
	int64_t index;
};
