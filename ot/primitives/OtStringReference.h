//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>

#include "OtReference.h"
#include "OtString.h"


//
//	OtStringReference
//

class OtStringReferenceClass;
using OtStringReference = OtObjectPointer<OtStringReferenceClass>;

class OtStringReferenceClass : public OtReferenceClass {
public:
	// constructors
	OtStringReferenceClass() = default;
	OtStringReferenceClass(OtString s, size_t i) : string(s), index(i) {}

	// debugging support
	std::string describe() override {
		return "[" + std::to_string(index) + "]";
	}

	// (de)reference functions
	std::string deref() {
		return string->getEntry(index);
	}

	OtObject assign(OtObject value) {
		return OtString::create(string->setEntry(
			index,
			value->operator std::string()));
	}

	// get type definition
	static OtType getMeta();

private:
	OtString string;
	size_t index;
};
