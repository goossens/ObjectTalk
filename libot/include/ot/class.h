//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "internal.h"


//
//	OtClass
//

class OtClassClass : public OtInternalClass {
public:
	// constructors
	OtClassClass() = default;
	OtClassClass(OtType t) : classType(t) {}

	// debugging support
	std::string describe() { return classType->getName(); }

	// access member information
	std::string getName() { return classType->getName(); }
	bool hasParent() { return classType->getParent() != nullptr; }
	OtObject getParent() { return OtClassClass::create(classType->getParent()); }

	// instantiate a new class instance
	OtObject instantiate(size_t count, OtObject* parameters);

	// create a sub class
	OtObject subClass(const std::string& name) { return OtClassClass::create(classType->subType(name)); }

	// see if class is kind of
	bool isKindOf(const std::string& className) { return classType->isKindOf(className); }

	// special member acccess (so we can manipulate metaclass members via class)
	OtObject set(const std::string& name, OtObject value) { return classType->set(name, value); }
	void unset(const std::string& name) { return classType->unset(name); }

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtClass create(OtType type);

protected:
	OtType classType;
};
