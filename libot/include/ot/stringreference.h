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

#include "string.h"
#include "reference.h"


//
//	OtStringReference
//

class OtStringReferenceClass;
typedef std::shared_ptr<OtStringReferenceClass> OtStringReference;

class OtStringReferenceClass : public OtReferenceClass {
public:
	// constructors
	OtStringReferenceClass() = default;
	OtStringReferenceClass(OtString s, size_t i) : string(s), index(i) {}

	// debugging support
	std::string describe() { return "[" + std::to_string(index) + "]"; }

	// (de)reference functions
	std::string deref() { return string->getEntry(index); }

	OtObject assign(OtObject value) {
		return OtStringClass::create(string->setEntry(
			index,
			value->operator std::string()));
	}

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtStringReference create(OtString string, size_t index);

private:
	OtString string;
	size_t index;
};
