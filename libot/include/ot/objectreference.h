//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "reference.h"


//
//	OtObjectReference
//

class OtObjectReferenceClass;
typedef std::shared_ptr<OtObjectReferenceClass> OtObjectReference;

class OtObjectReferenceClass : public OtReferenceClass {
public:
	// constructors
	OtObjectReferenceClass() = default;
	OtObjectReferenceClass(OtObject o, const std::string& m) : object(o), member(m) {}

	// debugging support
	std::string describe() { return object->getType()->getName() + " " + member; }

	// (de)reference functions
	OtObject deref() { return object->get(member); }
	OtObject assign(OtObject value) { return object->set(member, value); }

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtObjectReference create(OtObject o, const std::string& m);

private:
	OtObject object;
	std::string member;
};
