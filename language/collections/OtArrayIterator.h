//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>

#include "OtArray.h"
#include "OtIterator.h"


//
//	OtArrayIteratorClass
//

class OtArrayIteratorClass;
using OtArrayIterator = OtObjectPointer<OtArrayIteratorClass>;

class OtArrayIteratorClass : public OtIteratorClass {
public:
	// iteration operations
	inline bool end() { return index == array->size(); }
	inline OtObject next() { return array->getEntry(index++); }

	// get type definition
	static OtType getMeta();

protected:
	// constructors
	friend class OtObjectPointer<OtArrayIteratorClass>;
	OtArrayIteratorClass() = default;
	OtArrayIteratorClass(OtArray a) : array(a) {}

private:
	// data
	OtArray array;
	size_t index {0};
};
