//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtReference.h"
#include "OtSelector.h"


//
//	OtObjectReference
//

class OtObjectReferenceClass;
using OtObjectReference = OtObjectPointer<OtObjectReferenceClass>;

class OtObjectReferenceClass : public OtReferenceClass {
public:
	// constructors
	OtObjectReferenceClass() = default;
	OtObjectReferenceClass(OtObject o, size_t m) : object(o), member(m) {}

	// debugging support
	std::string describe() override { return object->getType()->getName() + " " + OtSelector::name(member); }

	// (de)reference functions
	OtObject deref() { return object->get(member); }
	OtObject assign(OtObject value) { return object->set(member, value); }

	// get type definition
	static OtType getMeta();

private:
	OtObject object;
	size_t member;
};
