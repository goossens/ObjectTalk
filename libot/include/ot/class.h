//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "context.h"


//
//	OtClass
//

class OtClassClass : public OtContextClass {
public:
	// constructors
	OtClassClass() = default;
	OtClassClass(OtType t) : classType(t) {}

	// access member information
	std::string getName() { return classType->getName(); }
	bool hasParent() { return classType->getParent() != nullptr; }
	OtObject getParent() { return OtClassClass::create(classType->getParent()); }

	// create a sub class
	OtObject subClass(const std::string& name) { return OtClassClass::create(classType->subType(name)); }

	// see if class is kind of
	bool isKindOf(const std::string& className) { return classType->isKindOf(className); }

	// special member acccess (so we can add methods to metaclass via class
	OtObject set(const std::string& name, OtObject value) { return classType->set(name, value); }

	// call operator
	OtObject operator () (OtContext context, size_t count, OtObject* parameters);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtClass create(OtType type);

protected:
	OtType classType;
};


// OtObject OtObjectCreate(OtType type) { return OtClassClass::create(type); };
