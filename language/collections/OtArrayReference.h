//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
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
	// debugging support
	inline std::string describe() override { return "[" + std::to_string(index) + "]"; }

	// (de)reference functions
	inline OtObject deref() { return array->getEntry(index); }
	inline OtObject assign(OtObject object) { array->setEntry(index, object); return object; }

	// get type definition
	static OtType getMeta();

protected:
	// constructors
	friend class OtObjectPointer<OtArrayReferenceClass>;
	OtArrayReferenceClass() = default;
	OtArrayReferenceClass(OtArray a, size_t i) : array(a), index(i) {}

private:
	// data
	OtArray array;
	size_t index;
};
