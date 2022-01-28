//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtInternal.h"


//
//	OtClass
//

class OtClassClass : public OtInternalClass {
public:
	// constructors
	OtClassClass() = default;
	OtClassClass(OtType t) : classType(t) {}
	OtClassClass(const std::string& name);

	// debugging support
	std::string describe() { return classType->getName(); }

	// access member information
	void setParent(OtObject parent);
	std::string getName() { return classType->getName(); }
	bool hasParent() { return classType->getParent() != nullptr; }
	OtObject getParent() { return OtClassClass::create(classType->getParent()); }

	// create a new class instance
	OtObject instantiate(size_t count, OtObject* parameters);

	// see if class is kind of
	bool isKindOf(const std::string& className) { return classType->isKindOf(className); }

	// special member acccess (so we can manipulate metaclass members via class)	virtual OtObject get(const std::string& name);
	bool has(const std::string& name);
	OtObject get(const std::string& name);
	OtObject set(const std::string& name, OtObject value) { return classType->set(name, value); }
	void unset(const std::string& name) { return classType->unset(name); }

	// get the classes type
	OtType getClassType() { return classType; }

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtClass create(const std::string& name);
	static OtClass create(OtType type);

protected:
	OtType classType;
};
