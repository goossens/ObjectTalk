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
#include "OtReference.h"


//
//	OtArrayReferenceClass
//

class OtArrayReferenceClass;
using OtArrayReference = OtObjectPointer<OtArrayReferenceClass>;

class OtArrayReferenceClass : public OtReferenceClass {
public:
	// constructors
	OtArrayReferenceClass() = default;
	OtArrayReferenceClass(OtArray a, size_t i) : array(a), index(i) {}

	// debugging support
	inline std::string describe() override { return "[" + std::to_string(index) + "]"; }

	// (de)reference functions
	inline OtObject deref() { return array->getEntry(index); }
	inline OtObject assign(OtObject object) { array->setEntry(index, object); return object; }

	// get type definition
	static OtType getMeta();

private:
	OtArray array;
	size_t index;
};
