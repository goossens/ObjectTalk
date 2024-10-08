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

#include "OtInternal.h"
#include "OtType.h"


//
//	OtClass
//

class OtClassClass : public OtInternalClass {
public:
	// constructors
	OtClassClass() = default;
	OtClassClass(OtType t) : classType(t) {}
	OtClassClass(OtID id) { classType = OtType::create(id); }

	// debugging support
	inline std::string describe() override { return classType->getName(); }

	// access member information
	void setParent(OtObject parent);
	inline std::string getName() { return classType->getName(); }
	inline bool hasParent() { return classType->getParent(); }
	inline OtObject getParent() { return OtClass::create(classType->getParent()); }

	// create a new class instance
	OtObject instantiate(size_t count, OtObject* parameters);

	// see if class is kind of
	inline bool isKindOf(const std::string& className) { return classType->isKindOf(className); }

	// special member acccess (so we can manipulate metaclass members via class)
	inline bool has(OtID id) override { return OtInternalClass::has(id) ? true : classType->has(id); }
	inline OtObject& get(OtID id) override { return OtInternalClass::has(id) ? OtInternalClass::get(id) : classType->get(id); }
	inline OtObject set(OtID id, OtObject value) override { return classType->set(id, value); }
	inline void unset(OtID id) override { return classType->unset(id); }

	// special superclass member access
	OtObject getSuper(OtID id);

	// get the classes type
	inline OtType getClassType() { return classType; }

	// get type definition
	static OtType getMeta();

protected:
	OtType classType;
};
