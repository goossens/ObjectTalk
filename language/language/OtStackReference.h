//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
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
	// debugging support
	inline std::string describe() override {
		std::string name(OtIdentifier::name(id));
		return name + " (slot " + std::to_string(slot) + ")";
	}

	// (de)reference functions
	OtObject deref();
	OtObject assign(OtObject value);

	// access members
	inline OtID getID() { return id; }
	inline size_t getSlot() { return slot; }

	// get type definition
	static OtType getMeta();

protected:
	// constructors
	friend class OtObjectPointer<OtStackReferenceClass>;
	OtStackReferenceClass() = default;
	OtStackReferenceClass(OtID i, size_t s) : id(i), slot(s) {}

private:
	// data
	OtID id;
	size_t slot;
};
