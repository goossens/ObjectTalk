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

#include "OtReference.h"
#include "OtStack.h"


//
// OtStackReferenceClass
//

class OtStackReferenceClass;
typedef std::shared_ptr<OtStackReferenceClass> OtStackReference;

class OtStackReferenceClass : public OtReferenceClass {
public:
	// constructors
	OtStackReferenceClass() = default;
	OtStackReferenceClass(const std::string& n, size_t s) : name(n), slot(s) {}

	// debugging support
	std::string describe() override { return name + " slot " + std::to_string(slot); }

	// (de)reference functions
	OtObject deref();
	OtObject assign(OtObject value);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtStackReference create(const std::string& name, size_t slot);

private:
	std::string name;
	size_t slot;
};
