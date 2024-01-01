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
#include "OtStack.h"


//
// OtStackReferenceClass
//

class OtStackReferenceClass;
using OtStackReference = OtObjectPointer<OtStackReferenceClass>;

class OtStackReferenceClass : public OtReferenceClass {
public:
	// constructors
	OtStackReferenceClass() = default;
	OtStackReferenceClass(const std::string& n, size_t s) : name(n), slot(s) {}

	// debugging support
	std::string describe() override { return name + " (slot " + std::to_string(slot) + ")"; }

	// (de)reference functions
	OtObject deref();
	OtObject assign(OtObject value);

	// access members
	std::string& getName() { return name; }
	size_t getSlot() { return slot; }

	// get type definition
	static OtType getMeta();

private:
	std::string name;
	size_t slot;
};
