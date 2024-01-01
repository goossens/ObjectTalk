//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
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
	std::string describe() override { return classType->getName(); }

	// access member information
	void setParent(OtObject parent);
	std::string getName() { return classType->getName(); }
	bool hasParent() { return classType->getParent(); }
	OtObject getParent() { return OtClass::create(classType->getParent()); }

	// create a new class instance
	OtObject instantiate(size_t count, OtObject* parameters);

	// see if class is kind of
	bool isKindOf(const std::string& className) { return classType->isKindOf(className); }

	// special member acccess (so we can manipulate metaclass members via class)
	bool has(size_t selector) override;
	OtObject& get(size_t selector) override;
	OtObject set(size_t selector, OtObject value) override { return classType->set(selector, value); }
	void unset(size_t selector) override { return classType->unset(selector); }

	// get the classes type
	OtType getClassType() { return classType; }

	// get type definition
	static OtType getMeta();

protected:
	OtType classType;
};
