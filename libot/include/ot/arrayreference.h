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
#include "reference.h"


//
//	OtArrayReferenceClass
//

class OtArrayReferenceClass;
typedef std::shared_ptr<OtArrayReferenceClass> OtArrayReference;

class OtArrayReferenceClass : public OtReferenceClass {
public:
	// constructors
	OtArrayReferenceClass() = default;
	OtArrayReferenceClass(OtArray a, size_t i) : array(a), index(i) {}

	// debugging support
	std::string describe() { return "[" + std::to_string(index) + "]"; }

	// (de)reference functions
	OtObject deref() { return array->getEntry(index); }
	OtObject assign(OtObject object) { array->setEntry(index, object); return object; }

	// get type definition
	static OtType getMeta();

	// create a new array reference
	static OtArrayReference create(OtArray array, size_t index);

private:
	OtArray array;
	size_t index;
};
